/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include "fdpa_sm_agent.h"

#include "../../util/alg_ds/alg/defer.h"
#include "dec/fdpa_dec_generic.h"
#include "fdpa_sm_id.h"
#include "enc/fdpa_enc_generic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  fdpa_enc_asn_t enc;
#elif FLATBUFFERS 
  fdpa_enc_fb_t enc;
#elif PLAIN
  fdpa_enc_plain_t enc;
#else
  static_assert(false, "No encryptioin type selected");
#endif

} sm_fdpa_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
sm_ag_if_ans_subs_t on_subscription_fdpa_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_fdpa_agent_t* sm = (sm_fdpa_agent_t*)sm_agent;
 
  fdpa_event_trigger_t ev = fdpa_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  sm_ag_if_ans_subs_t ans = {.type = PERIODIC_SUBSCRIPTION_FLRC}; 
  ans.per.t.ms = ev.ms;
  return ans;
//  const sm_wr_if_t wr = {.type = SUBSCRIBE_TIMER, .sub_timer = timer };
//  sm->base.io.write(&wr);
//  printf("on_subscription called with event trigger = %u \n", ev.ms);
}

// New includes for state/action handling
#include "fdpa_data_ie.h"
#include <math.h>

// Helper to create fdpa_state_t from simulation variables (placeholder)
static fdpa_state_t* build_fdpa_state(void) {
    // Example values – to be replaced with actual simulation data
    uint32_t N = 1; // number of gNBs (for rfsim single cell)
    uint32_t M = 8; // number of RBs per gNB (example)
    uint32_t Kmax = 4; // top K CQI per RB
    size_t sz = sizeof(fdpa_state_t) + N * sizeof(float) + N * M * Kmax * sizeof(float);
    fdpa_state_t* state = (fdpa_state_t*)malloc(sz);
    assert(state != NULL);
    state->n = N;
    state->m = M;
    state->kmax = Kmax;
    // Populate pu_norm with dummy normalized power values
    for(uint32_t i = 0; i < N; ++i) {
        state->pu_norm[i] = 1.0f; // placeholder
    }
    // Populate CQI values after pu_norm
    float* cqi = (float*)(state->pu_norm + N);
    for(uint32_t i = 0; i < N * M * Kmax; ++i) {
        cqi[i] = 10.0f; // placeholder CQI
    }
    return state;
}

// Helper to apply fdpa_action_t to the simulation (placeholder)
static void apply_fdpa_action(fdpa_action_t const* act) {
    // TODO: integrate with the power allocation matrix in the simulator
    // Example: iterate over N and M and set power values
    // This function is a stub for now.
    (void)act; // silence unused warning
}

// Modified on_indication to send state IE
static
exp_ind_data_t on_indication_fdpa_sm_ag(sm_agent_t const* sm_agent, void* act_def)
{
    assert(sm_agent != NULL);
    assert(act_def == NULL && "Action definition data not needed for this SM");
    sm_fdpa_agent_t* sm = (sm_fdpa_agent_t*)sm_agent;

    exp_ind_data_t ret = {.has_value = true};

    // Fill Indication Header
    fdpa_ind_hdr_t hdr = {.dummy = 0 };
    byte_array_t ba_hdr = fdpa_enc_ind_hdr(&sm->enc, &hdr);
    ret.data.ind_hdr = ba_hdr.buf;
    ret.data.len_hdr = ba_hdr.len;

    // Build state structure and encode as indication message
    fdpa_state_t* st = build_fdpa_state();
    // Encode using generic byte array (direct copy) – the decoder will reconstruct the struct
    byte_array_t ba_state = {.len = sizeof(fdpa_state_t) + st->n * sizeof(float) + st->n * st->m * st->kmax * sizeof(float), .buf = (uint8_t*)st};
    ret.data.ind_msg = ba_state.buf;
    ret.data.len_msg = ba_state.len;

    // No Call Process ID used
    ret.data.call_process_id = NULL;
    ret.data.len_cpid = 0;

    // Clean up temporary state (RAN will copy it)
    free_fdpa_state(st);
    return ret;
}

// Modified on_control to receive full action payload
static
sm_ctrl_out_data_t on_control_fdpa_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
    assert(sm_agent != NULL);
    assert(data != NULL);
    sm_fdpa_agent_t* sm = (sm_fdpa_agent_t*) sm_agent;

    // Decode action payload (now fdpa_action_t via cp_fdpa_ctrl_msg)
    fdpa_action_t* action = cp_fdpa_ctrl_msg((fdpa_ctrl_msg_t*)data->ctrl_msg);
    // Apply the action to the simulation (placeholder implementation)
    apply_fdpa_action(action);
    // Free the received action after use
    free_fdpa_action(action);

    // Respond with empty control outcome (keep existing behaviour)
    sm_ctrl_out_data_t ret = {0};
    ret.len_out = 0;
    ret.ctrl_out = NULL;
    return ret;
}

static
sm_ctrl_out_data_t on_control_fdpa_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_fdpa_agent_t* sm = (sm_fdpa_agent_t*) sm_agent;

  fdpa_ctrl_hdr_t hdr = fdpa_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  assert(hdr.dummy == 1 && "Only dummy == 1 supported ");

  fdpa_ctrl_msg_t msg = fdpa_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  assert(msg.action == 42 && "Only action number 42 supported");

//  sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
//  wr.ctrl.type = FDPA_CTRL_REQ_V0; 

  fdpa_ctrl_req_data_t fdpa_ctrl = {0};
  fdpa_ctrl.hdr.dummy = hdr.dummy;
  fdpa_ctrl.msg.action = msg.action;

  sm->base.io.write_ctrl(&fdpa_ctrl);
  sm_ctrl_out_data_t ret = {0};
  ret.len_out = 0;
  ret.ctrl_out = NULL;

  //printf("on_control called \n");
  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_fdpa_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
//  printf("[E2SM FDPA] on_e2_setup called \n");

  // Fill E2 Setup Request 
  // sm_ag_if_rd_t rd_if = {.type = E2_SETUP__AGENT_IF_ANS_V0};
  // rd_if.e2ap.type = FDPA_AGENT_IF_E2_SETUP_ANS_V0;

  // This may allocate memory by the RAN
  // sm->base.io.read(&rd_if);
  // Liberate the memory if previously allocated by the RAN. It sucks. Profoundly
  //fdpa_e2_setup_data_t* setup = &rd_if.e2ap.fdpa;

  sm_fdpa_agent_t* sm = (sm_fdpa_agent_t*)sm_agent;
  (void)sm;

  sm_e2_setup_data_t setup = {.len_rfd = 0, .ran_fun_def = NULL }; 

  size_t const sz = strnlen(SM_FDPA_STR, 256);
  assert(sz < 256 && "Buffer overeflow?");

  setup.len_rfd = sz;
  setup.ran_fun_def = calloc(1, sz);
  assert(setup.ran_fun_def != NULL);

  memcpy(setup.ran_fun_def, SM_FDPA_STR , sz);
 
  /*
  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));

  // RAN Function
  setup.rf.def = cp_str_to_ba(SM_FDPA_SHORT_NAME);
  setup.rf.id = SM_FDPA_ID;
  setup.rf.rev = SM_FDPA_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_FDPA_OID);
*/

  return setup;
}

static
 sm_ric_service_update_data_t on_ric_service_update_fdpa_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0}; 
  return dst;
}

static
void free_fdpa_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_fdpa_agent_t* sm = (sm_fdpa_agent_t*)sm_agent;
  free(sm);
}


// General SM information

// Definition
static
char const* def_fdpa_sm_ag(void)
{
  return SM_FDPA_STR;
}

// ID
static
uint16_t id_fdpa_sm_ag(void)
{
  return SM_FDPA_ID; 
}

  // Revision
static
uint16_t rev_fdpa_sm_ag (void)
{
  return SM_FDPA_REV;
}

// OID
static
char const* oid_fdpa_sm_ag (void)
{
  return SM_FDPA_OID;
}




sm_agent_t* make_fdpa_sm_agent(sm_io_ag_ran_t io)
{
  sm_fdpa_agent_t* sm = calloc(1, sizeof(sm_fdpa_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[FDPA_STATS_V0];
  sm->base.io.read_setup = io.read_setup_tbl[FDPA_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[FDPA_CTRL_REQ_V0];
  sm->base.io.write_subs = io.write_subs_tbl[FDPA_SUBS_V0];

  sm->base.free_sm = free_fdpa_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_fdpa_sm_ag;

  sm->base.proc.on_subscription = on_subscription_fdpa_sm_ag;
  sm->base.proc.on_indication = on_indication_fdpa_sm_ag;
  sm->base.proc.on_control = on_control_fdpa_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_fdpa_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_fdpa_sm_ag;
  sm->base.handle = NULL;

  // General SM information
  sm->base.info.def = def_fdpa_sm_ag;
  sm->base.info.id =  id_fdpa_sm_ag;
  sm->base.info.rev = rev_fdpa_sm_ag;
  sm->base.info.oid = oid_fdpa_sm_ag;



  //*(uint16_t*)(&sm->base.ran_func_id) = SM_FDPA_ID; 
  //assert(strlen( SM_FDPA_STR ) < sizeof(sm->base.ran_func_name));
  //memcpy(sm->base.ran_func_name, SM_FDPA_STR, strlen(SM_FDPA_STR));

  return &sm->base;
}

/*
uint16_t id_fdpa_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_fdpa_agent_t* sm = (sm_fdpa_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}
*/

