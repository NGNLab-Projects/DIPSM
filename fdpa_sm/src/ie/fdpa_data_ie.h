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


#ifndef FDPA_DATA_INFORMATION_ELEMENTS_H
#define FDPA_DATA_INFORMATION_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 9 Information Elements (IE) , RIC Event Trigger Definition, RIC Action Definition, RIC Indication Header, RIC Indication Message, RIC Call Process ID, RIC Control Header, RIC Control Message, RIC Control Outcome and RAN Function Definition defined by ORAN-WG3.E2SM-v01.00.00 at Section 5
 */


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

typedef struct {
  uint32_t ms;
} fdpa_event_trigger_t;

void free_fdpa_event_trigger(fdpa_event_trigger_t* src); 

fdpa_event_trigger_t cp_fdpa_event_trigger( fdpa_event_trigger_t const* src);

bool eq_fdpa_event_trigger(fdpa_event_trigger_t const* m0, fdpa_event_trigger_t const* m1);



//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////


typedef struct {
  uint32_t dummy;  
} fdpa_action_def_t;

void free_fdpa_action_def(fdpa_action_def_t* src); 

fdpa_action_def_t cp_fdpa_action_def(fdpa_action_def_t* src);

bool eq_fdpa_action_def(fdpa_event_trigger_t* m0,  fdpa_event_trigger_t* m1);



//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


typedef struct{
  uint32_t dummy;  
} fdpa_ind_hdr_t;

void free_fdpa_ind_hdr(fdpa_ind_hdr_t* src); 

fdpa_ind_hdr_t cp_fdpa_ind_hdr(fdpa_ind_hdr_t const* src);

bool eq_fdpa_ind_hdr(fdpa_ind_hdr_t* m0, fdpa_ind_hdr_t* m1);

//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////

typedef struct
{
  uint64_t dl_aggr_tbs;
  uint64_t ul_aggr_tbs;
  uint64_t dl_aggr_bytes_sdus;
  uint64_t ul_aggr_bytes_sdus;
  uint64_t dl_curr_tbs;
  uint64_t ul_curr_tbs;
  uint64_t dl_sched_rb;
  uint64_t ul_sched_rb;
 
  float pusch_snr; //: float = -64;
  float pucch_snr; //: float = -64;

  float dl_bler;
  float ul_bler;

  uint32_t dl_harq[5];
  uint32_t ul_harq[5];
  uint32_t dl_num_harq;
  uint32_t ul_num_harq;

  uint32_t rnti;
  uint32_t dl_aggr_prb; 
  uint32_t ul_aggr_prb;
  uint32_t dl_aggr_sdus;
  uint32_t ul_aggr_sdus;
  uint32_t dl_aggr_retx_prb;
  uint32_t ul_aggr_retx_prb;

  uint32_t bsr;
  uint16_t frame;
  uint16_t slot;

  uint8_t wb_cqi; 
  uint8_t dl_mcs1;
  uint8_t ul_mcs1;
  uint8_t dl_mcs2; 
  uint8_t ul_mcs2; 
  int8_t phr; 

} fdpa_ue_stats_impl_t;

fdpa_ue_stats_impl_t cp_fdpa_ue_stats_impl(fdpa_ue_stats_impl_t const* src);

typedef struct {
  uint32_t len_ue_stats;
  fdpa_ue_stats_impl_t* ue_stats;
  int64_t tstamp;
} fdpa_ind_msg_t;

void free_fdpa_ind_msg(fdpa_ind_msg_t* src); 

fdpa_ind_msg_t cp_fdpa_ind_msg(fdpa_ind_msg_t const* src);

bool eq_fdpa_ind_msg(fdpa_ind_msg_t* m0, fdpa_ind_msg_t* m1);


//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

typedef struct {
  uint32_t dummy;
} fdpa_call_proc_id_t;

void free_fdpa_call_proc_id( fdpa_call_proc_id_t* src); 

fdpa_call_proc_id_t cp_fdpa_call_proc_id( fdpa_call_proc_id_t* src);

bool eq_fdpa_call_proc_id(fdpa_call_proc_id_t* m0, fdpa_call_proc_id_t* m1);

//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////

typedef struct {
  uint32_t dummy;
} fdpa_ctrl_hdr_t;

void free_fdpa_ctrl_hdr( fdpa_ctrl_hdr_t* src); 

fdpa_ctrl_hdr_t cp_fdpa_ctrl_hdr(fdpa_ctrl_hdr_t* src);

bool eq_fdpa_ctrl_hdr(fdpa_ctrl_hdr_t* m0, fdpa_ctrl_hdr_t* m1);

//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////

// Updated control message to carry full action payload
typedef fdpa_action_t fdpa_ctrl_msg_t;

/* Prototypes for control message handling (reuse action helpers) */
void free_fdpa_ctrl_msg(fdpa_ctrl_msg_t* src) { free_fdpa_action(src); }
fdpa_ctrl_msg_t* cp_fdpa_ctrl_msg(const fdpa_ctrl_msg_t* src) { return cp_fdpa_action(src); }
bool eq_fdpa_ctrl_msg(const fdpa_ctrl_msg_t* m0, const fdpa_ctrl_msg_t* m1) { return eq_fdpa_action(m0, m1); }


//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

typedef enum{
  FDPA_CTRL_OUT_OK,


  FDPA_CTRL_OUT_END
} fdpa_ctrl_out_e;

typedef struct {
  fdpa_ctrl_out_e ans;  
} fdpa_ctrl_out_t;

void free_fdpa_ctrl_out(fdpa_ctrl_out_t* src); 

fdpa_ctrl_out_t cp_fdpa_ctrl_out(fdpa_ctrl_out_t* src);

bool eq_fdpa_ctrl_out(fdpa_ctrl_out_t* m0, fdpa_ctrl_out_t* m1);


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

/* New structs for FDPA reinforcement learning */
typedef struct {
  uint32_t n;               // number of gNBs (N)
  uint32_t m;               // number of RBs per gNB (M)
  uint32_t kmax;            // top Kmax CQI per RB
  float pu_norm[];           // flexible array: N normalized power values
  // cqi values will follow pu_norm in memory: N * M * Kmax floats
  // they can be accessed as: float* cqi = (float*)(pu_norm + n);
} fdpa_state_t;

/* Prototypes for state handling */
void free_fdpa_state(fdpa_state_t* src);
fdpa_state_t* cp_fdpa_state(const fdpa_state_t* src);
bool eq_fdpa_state(const fdpa_state_t* m0, const fdpa_state_t* m1);

typedef struct {
  uint32_t n;               // number of gNBs (N)
  uint32_t m;               // number of RBs per gNB (M)
  float power[];           // flexible array: N * M power allocation values
} fdpa_action_t;

/* Prototypes for action handling */
void free_fdpa_action(fdpa_action_t* src);
fdpa_action_t* cp_fdpa_action(const fdpa_action_t* src);
bool eq_fdpa_action(const fdpa_action_t* m0, const fdpa_action_t* m1);



typedef struct {
  size_t len;
  uint8_t* buf;
} fdpa_func_def_t;

void free_fdpa_func_def(fdpa_func_def_t* src); 

fdpa_func_def_t cp_fdpa_func_def(fdpa_func_def_t const* src);

bool eq_fdpa_func_def(fdpa_func_def_t const* m0, fdpa_func_def_t const* m1);


/////////////////////////////////////////////////
//////////////////////////////////////////////////
/////////////////////////////////////////////////


/*
 * O-RAN defined 5 Procedures: RIC Subscription, RIC Indication, RIC Control, E2 Setup and RIC Service Update 
 * */


///////////////
/// RIC Subscription
///////////////

typedef struct{
  fdpa_event_trigger_t et; 
  fdpa_action_def_t* ad;
} fdpa_sub_data_t;

///////////////
// RIC Indication
///////////////

typedef struct{
  fdpa_ind_hdr_t hdr;
  fdpa_ind_msg_t msg;
  fdpa_call_proc_id_t* proc_id;
} fdpa_ind_data_t;

fdpa_ind_data_t cp_fdpa_ind_data( fdpa_ind_data_t const* src);

void free_fdpa_ind_data(fdpa_ind_data_t* ind);

///////////////
// RIC Control
///////////////

typedef struct{
  fdpa_ctrl_hdr_t hdr;
  fdpa_ctrl_msg_t msg;
} fdpa_ctrl_req_data_t;

typedef struct{
  fdpa_ctrl_out_t* out;
} fdpa_ctrl_out_data_t;

///////////////
// E2 Setup
///////////////

typedef struct{
  fdpa_func_def_t func_def;
} fdpa_e2_setup_data_t;

///////////////
// RIC Service Update
///////////////

typedef struct{
  fdpa_func_def_t func_def;
} fdpa_ric_service_update_t;

#ifdef __cplusplus
}
#endif




#endif
