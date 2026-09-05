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


#include "fdpa_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../../../util/alg_ds/alg/eq_float.h"


//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

void free_fdpa_event_trigger(fdpa_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

fdpa_event_trigger_t cp_fdpa_event_trigger( fdpa_event_trigger_t const* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 

  fdpa_event_trigger_t et = {0};
  return et;
}

bool eq_fdpa_event_trigger(fdpa_event_trigger_t const* m0, fdpa_event_trigger_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

void free_fdpa_action_def(fdpa_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

fdpa_action_def_t cp_fdpa_action_def(fdpa_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  fdpa_action_def_t ad = {0};
  return ad;
}

bool eq_fdpa_action_def(fdpa_event_trigger_t* m0,  fdpa_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_fdpa_ind_hdr(fdpa_ind_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
}

fdpa_ind_hdr_t cp_fdpa_ind_hdr(fdpa_ind_hdr_t const* src)
{
  assert(src != NULL);
  fdpa_ind_hdr_t dst = {0}; 
  dst.dummy = src->dummy;
  return dst;
}

bool eq_fdpa_ind_hdr(fdpa_ind_hdr_t* m0, fdpa_ind_hdr_t* m1)
{
  assert(m0 != 0);
  assert(m1 != 0);

  if(m0->dummy != m1->dummy)
    return false;
  return true;
}


//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////

void free_fdpa_ind_msg(fdpa_ind_msg_t* src)
{
  assert(src != NULL);
  if(src->len_ue_stats > 0){
    assert(src->ue_stats != NULL);
    free(src->ue_stats);
  }
}

fdpa_ue_stats_impl_t cp_fdpa_ue_stats_impl(fdpa_ue_stats_impl_t const* src)
{
  assert(src != NULL);

  fdpa_ue_stats_impl_t dst = { .dl_aggr_tbs = src->dl_aggr_tbs,
                              .ul_aggr_tbs = src->ul_aggr_tbs,
                              .dl_aggr_bytes_sdus = src->dl_aggr_bytes_sdus,
                              .ul_aggr_bytes_sdus = src->ul_aggr_bytes_sdus,
                              .dl_curr_tbs = src->dl_curr_tbs,
                              .ul_curr_tbs = src->ul_curr_tbs,
                              .dl_sched_rb = src->dl_sched_rb,
                              .ul_sched_rb = src->ul_sched_rb,

                              .pusch_snr = src->pusch_snr, //: float = -64;
                              .pucch_snr = src->pucch_snr, //: float = -64;

                              .rnti = src->rnti,
                              .dl_aggr_prb = src->dl_aggr_prb, 
                              .ul_aggr_prb = src->ul_aggr_prb,
                              .dl_aggr_sdus = src->dl_aggr_sdus,
                              .ul_aggr_sdus = src->ul_aggr_sdus,
                              .dl_aggr_retx_prb = src->dl_aggr_retx_prb,
                              .ul_aggr_retx_prb = src->ul_aggr_retx_prb,

                              .wb_cqi = src->wb_cqi, 
                              .dl_mcs1 = src->dl_mcs1,
                              .ul_mcs1 = src->ul_mcs1,
                              .dl_mcs2 = src->dl_mcs2, 
                              .ul_mcs2 = src->ul_mcs2, 
                              .phr = src->phr,
                              .bsr = src->bsr,
                              .dl_bler = src->dl_bler,
                              .ul_bler = src->ul_bler,
                              .dl_num_harq = src->dl_num_harq,
                              .dl_harq[0] = src->dl_harq[0],
                              .dl_harq[1] = src->dl_harq[1],
                              .dl_harq[2] = src->dl_harq[2],
                              .dl_harq[3] = src->dl_harq[3],
                              .dl_harq[4] = src->dl_harq[4],
                              .ul_num_harq = src->ul_num_harq,
                              .ul_harq[0] = src->ul_harq[0],
                              .ul_harq[1] = src->ul_harq[1],
                              .ul_harq[2] = src->ul_harq[2],
                              .ul_harq[3] = src->ul_harq[3],
                              .ul_harq[4] = src->ul_harq[4],
                              .frame = src->frame,
                              .slot = src->slot
                            }; 

  return dst;
}

fdpa_ind_msg_t cp_fdpa_ind_msg( fdpa_ind_msg_t const* src)
{
  assert(src != NULL);

  fdpa_ind_msg_t dst = {0};

  dst.len_ue_stats = src->len_ue_stats;
  if(dst.len_ue_stats > 0){
    dst.ue_stats = calloc(dst.len_ue_stats, sizeof( fdpa_ue_stats_impl_t) );
    assert(dst.ue_stats != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.len_ue_stats; ++i){
    dst.ue_stats[i] = cp_fdpa_ue_stats_impl(&src->ue_stats[i]); 
  }

  //memcpy(ret.ue_stats, src->ue_stats, sizeof( fdpa_ue_stats_impl_t )*ret.len_ue_stats);

  dst.tstamp = src->tstamp; 

  return dst;
}

bool eq_fdpa_ind_msg(fdpa_ind_msg_t* m0, fdpa_ind_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_ue_stats != m1->len_ue_stats || m0->tstamp != m1->tstamp)
    return false;

  for(uint32_t i = 0 ; i < m0->len_ue_stats; ++i){
    fdpa_ue_stats_impl_t* ue0 = &m0->ue_stats[i]; 
    fdpa_ue_stats_impl_t* ue1 = &m1->ue_stats[i]; 

    if(
        ue0->dl_aggr_tbs != ue1->dl_aggr_tbs ||
        ue0->ul_aggr_tbs != ue1->ul_aggr_tbs ||
        ue0->dl_aggr_bytes_sdus != ue1->dl_aggr_bytes_sdus ||
        ue0->ul_aggr_bytes_sdus != ue1->ul_aggr_bytes_sdus ||
        ue0->dl_curr_tbs != ue1->dl_curr_tbs ||
        ue0->ul_curr_tbs != ue1->ul_curr_tbs ||
        ue0->dl_sched_rb != ue1->dl_sched_rb ||
        ue0->ul_sched_rb != ue1->ul_sched_rb ||
        ue0->rnti != ue1->rnti ||
        ue0->dl_aggr_prb != ue1->dl_aggr_prb ||  
        ue0->ul_aggr_prb != ue1->ul_aggr_prb ||
        ue0->dl_aggr_sdus != ue1-> dl_aggr_sdus ||
        ue0->ul_aggr_sdus != ue1->ul_aggr_sdus ||
        ue0->dl_aggr_retx_prb != ue1->dl_aggr_retx_prb ||
        ue0->ul_aggr_retx_prb != ue1->ul_aggr_retx_prb ||
        ue0->wb_cqi != ue1->wb_cqi || 
        ue0->dl_mcs1 != ue1->dl_mcs1 ||
        ue0->ul_mcs1 != ue1->ul_mcs1 ||
        ue0->dl_mcs2 != ue1->dl_mcs2 || 
        ue0->ul_mcs2 != ue1->ul_mcs2 || 
        ue0->phr != ue1->phr || 
        ue0->bsr != ue1->bsr ||
        ue0->dl_bler != ue1->dl_bler ||
        ue0->ul_bler != ue1->ul_bler ||
        ue0->dl_num_harq != ue1->dl_num_harq ||
        ue0->dl_harq[0] != ue1->dl_harq[0] ||
        ue0->dl_harq[1] != ue1->dl_harq[1] ||
        ue0->dl_harq[2] != ue1->dl_harq[2] ||
        ue0->dl_harq[3] != ue1->dl_harq[3] ||
        ue0->dl_harq[4] != ue1->dl_harq[4] ||
        ue0->ul_num_harq != ue1->ul_num_harq ||
        ue0->ul_harq[0] != ue1->ul_harq[0] ||
        ue0->ul_harq[1] != ue1->ul_harq[1] ||
        ue0->ul_harq[2] != ue1->ul_harq[2] ||
        ue0->ul_harq[3] != ue1->ul_harq[3] ||
        ue0->ul_harq[4] != ue1->ul_harq[4] ||
        ue0->frame != ue1->frame ||
        ue0->slot != ue1->slot ||
        eq_float(ue0->pusch_snr, ue1->pusch_snr, 0.0000001) == false ||
        eq_float(ue0->pucch_snr, ue0->pucch_snr, 0.0000001) == false 
      )
      return false;
  }
  return true;
}

//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

void free_fdpa_call_proc_id(fdpa_call_proc_id_t* src)
{
  // Note that the src could be NULL
  free(src);
}

fdpa_call_proc_id_t cp_fdpa_call_proc_id( fdpa_call_proc_id_t* src)
{
  assert(src != NULL); 
  fdpa_call_proc_id_t dst = {0};

  dst.dummy = src->dummy;

  return dst;
}

bool eq_fdpa_call_proc_id(fdpa_call_proc_id_t* m0, fdpa_call_proc_id_t* m1)
{
  if(m0 == NULL && m1 == NULL)
    return true;
  if(m0 == NULL)
    return false;
  if(m1 == NULL)
    return false;

  if(m0->dummy != m1->dummy)
    return false;

  return true;
}


//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////

void free_fdpa_ctrl_hdr( fdpa_ctrl_hdr_t* src)
{

  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

fdpa_ctrl_hdr_t cp_fdpa_ctrl_hdr(fdpa_ctrl_hdr_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
  fdpa_ctrl_hdr_t ret = {0};
  return ret;
}

bool eq_fdpa_ctrl_hdr(fdpa_ctrl_hdr_t* m0, fdpa_ctrl_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////


void free_fdpa_ctrl_msg( fdpa_ctrl_msg_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

fdpa_ctrl_msg_t cp_fdpa_ctrl_msg(fdpa_ctrl_msg_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  fdpa_ctrl_msg_t ret = {0};
  return ret;
}

bool eq_fdpa_ctrl_msg(fdpa_ctrl_msg_t* m0, fdpa_ctrl_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

void free_fdpa_ctrl_out(fdpa_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

fdpa_ctrl_out_t cp_fdpa_ctrl_out(fdpa_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  fdpa_ctrl_out_t ret = {0}; 
  return ret;
}

bool eq_fdpa_ctrl_out(fdpa_ctrl_out_t* m0, fdpa_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

/* Helper functions for fdpa_state_t */
void free_fdpa_state(fdpa_state_t* src) {
    assert(src != NULL);
    free(src);
}

fdpa_state_t* cp_fdpa_state(const fdpa_state_t* src) {
    assert(src != NULL);
    size_t sz = sizeof(fdpa_state_t) + src->n * sizeof(float);
    fdpa_state_t* dst = (fdpa_state_t*)malloc(sz);
    assert(dst != NULL);
    dst->n = src->n;
    dst->m = src->m;
    dst->kmax = src->kmax;
    memcpy(dst->pu_norm, src->pu_norm, src->n * sizeof(float));
    return dst;
}

bool eq_fdpa_state(const fdpa_state_t* a, const fdpa_state_t* b) {
    if (a == b) return true;
    if (a == NULL || b == NULL) return false;
    if (a->n != b->n || a->m != b->m || a->kmax != b->kmax) return false;
    for (uint32_t i = 0; i < a->n; ++i) {
        if (fabsf(a->pu_norm[i] - b->pu_norm[i]) > 1e-6f) return false;
    }
    return true;
}

/* Helper functions for fdpa_action_t */
void free_fdpa_action(fdpa_action_t* src) {
    assert(src != NULL);
    free(src);
}

fdpa_action_t* cp_fdpa_action(const fdpa_action_t* src) {
    assert(src != NULL);
    size_t sz = sizeof(fdpa_action_t) + src->n * src->m * sizeof(float);
    fdpa_action_t* dst = (fdpa_action_t*)malloc(sz);
    assert(dst != NULL);
    dst->n = src->n;
    dst->m = src->m;
    memcpy(dst->power, src->power, src->n * src->m * sizeof(float));
    return dst;
}

bool eq_fdpa_action(const fdpa_action_t* a, const fdpa_action_t* b) {
    if (a == b) return true;
    if (a == NULL || b == NULL) return false;
    if (a->n != b->n || a->m != b->m) return false;
    size_t total = a->n * a->m;
    for (size_t i = 0; i < total; ++i) {
        if (fabsf(a->power[i] - b->power[i]) > 1e-6f) return false;
    }
    return true;
}
{
  assert(src != NULL);
  free(src->buf);
}

fdpa_func_def_t cp_fdpa_func_def(fdpa_func_def_t const* src)
{
  assert(src != NULL);

  fdpa_func_def_t dst = {.len = src->len};
  if(src->len > 0){
    dst.buf = calloc(dst.len, sizeof(uint8_t)); 
    assert(dst.buf != NULL && "memory exhausted");
    memcpy(dst.buf, src->buf, dst.len);
  }

  return dst;
}

bool eq_fdpa_func_def(fdpa_func_def_t const* m0, fdpa_func_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->len != m1->len)
    return false;

  int rc = memcmp(m0, m1, m0->len);
  return rc == 0;
}

///////////////
// RIC Indication
///////////////

fdpa_ind_data_t cp_fdpa_ind_data( fdpa_ind_data_t const* src)
{
  assert(src != NULL);
  fdpa_ind_data_t dst = {0};
  dst.hdr = cp_fdpa_ind_hdr(&src->hdr);
  dst.msg = cp_fdpa_ind_msg(&src->msg);
  
  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(fdpa_call_proc_id_t)); 
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_fdpa_call_proc_id(src->proc_id);
  }

  return dst;
}

void free_fdpa_ind_data(fdpa_ind_data_t* ind)
{
  assert(ind != NULL);
  free_fdpa_ind_hdr(&ind->hdr);
  free_fdpa_ind_msg(&ind->msg);
  free_fdpa_call_proc_id(ind->proc_id);
}


