#include "mtwist.h"
#include <hls_math.h>
#include <math.h>
#include <ap_int.h>

//hls::stream<ap_uint<32> > hls_output_stream;
ap_uint<32> state[ui_cycles][ui_cycle_len];
ap_uint<10> p;

void seed(ap_uint<32> s) {  // init by 32 bit seed
	#pragma HLS ARRAY_PARTITION variable=state complete dim=1

  state[0][0] = s;// & 0xFFFFFFFFUL; // for > 32 bit machines
  //state[0] = s;
  for (ap_uint<10> i = 1; i < n; ++i) {
    ap_uint<10> i_subarr=i%cycles;
    ap_uint<10> i_index =i/cycles;

    ap_uint<10> i_m1_subarr=(i-1)%cycles;
    ap_uint<10> i_m1_index =(i-1)/cycles;

    state[i_subarr][i_index] = MT_f * (state[i_m1_subarr][i_m1_index] ^ (state[i_m1_subarr][i_m1_index] >> 30)) + i;
  }
}

void mtwist_core (bool init, ap_uint<32> seed_val, ap_uint<32> stream_length, ap_uint<32> *out_stream)//, uint16_t strm_len)
{
//	#pragma HLS INTERFACE axis port=out_stream depth=1

  if(init){
    seed(seed_val);
    p=0;
  }

  ap_uint<32> x;
  ap_uint<32> temp_state;
  ap_uint<32> temp_state_next;

  ap_uint<10> q=p;
  ap_uint<10> q_subarr=q%cycles;
  ap_uint<10> q_index=q/cycles;

  bool p_wrap_next= (p==n-2);
  ap_uint<10> p_next=(p+1);
  if(p==(n-1))
    p_next=0;
  ap_uint<10> p_subarr=p%cycles;
  ap_uint<10> p_index=p/cycles;

  ap_uint<10> index_cacheA;
  ap_uint<10> index_cacheA_subarr;
  ap_uint<10> index_cacheA_index;

  ap_uint<10> index_cacheC;
  ap_uint<10> index_cacheC_subarr;
  ap_uint<10> index_cacheC_index;

  ap_uint<32> state_cacheA;//=state[m];
  ap_uint<32> state_cacheB;//=state[0];
  ap_uint<32> state_cacheC;//=state[1];

  state_cacheB=state[p_subarr][p_index];

  if(p<(n-m)){
    index_cacheA=p+m;
    index_cacheC=p+1;
  } else if(p<(n-1) ){
    index_cacheA=p+m-n;
    index_cacheC=p+1;
  } else {
    index_cacheA=m-1;
    index_cacheC=0;
    //p=0;
  }
  index_cacheA_subarr=index_cacheA%cycles;
  index_cacheA_index=index_cacheA/cycles;
  index_cacheC_subarr=index_cacheC%cycles;
  index_cacheC_index=index_cacheC/cycles;

  state_cacheA=state[index_cacheA_subarr][index_cacheA_index];
  state_cacheC=state[index_cacheC_subarr][index_cacheC_index];

  temp_state_next=state_cacheA^ twiddle(state_cacheB, state_cacheC);

  ap_uint<10> index_cacheA_next=index_cacheA+1;
  if(index_cacheA==(n-1))
    index_cacheA_next=0;
  ap_uint<10> index_cacheC_next=index_cacheC+1;
  if(index_cacheC==(n-1))
    index_cacheC_next=0;

  bool index_cacheA_wrap_next=index_cacheA_next==n-1;
  bool index_cacheC_wrap_next=index_cacheC_next==n-1;


  bool stop_next=false;
  if(stream_length==0)
	stop_next=true;

  ap_uint<32> rand_index=0;

 rand_compute: while(true){//for(ap_uint<32> rand_index=0;rand_index<stream_length; rand_index++){
    #pragma HLS DEPENDENCE variable=state array inter WAR false

	#pragma HLS PIPELINE

	if(stop_next){
      break;
	}
    stop_next=(rand_index==(stream_length-1));

    rand_index++;

    temp_state=temp_state_next;
    x = state_cacheB;//state[p];

    q=p;
    p=p_next;
    p_subarr=p%cycles;
    p_index=p/cycles;

    state_cacheB=state_cacheC;

    index_cacheA=index_cacheA_next;
    index_cacheC=index_cacheC_next;

    index_cacheA_subarr=index_cacheA%cycles;
    index_cacheA_index=index_cacheA/cycles;

    index_cacheC_index=index_cacheC/cycles;

    switch(index_cacheA_subarr){
    case 0:
      state_cacheA=state[0][index_cacheA_index];
      state_cacheC=state[4][index_cacheC_index];
      state[2][q_index]=temp_state;
      break;
    case 1:
      state_cacheA=state[1][index_cacheA_index];
      state_cacheC=state[5][index_cacheC_index];
      state[3][q_index]=temp_state;
      break;
    case 2:
      state_cacheA=state[2][index_cacheA_index];
      state_cacheC=state[6][index_cacheC_index];
      state[4][q_index]=temp_state;
      break;
    case 3:
      state_cacheA=state[3][index_cacheA_index];
      state_cacheC=state[7][index_cacheC_index];
      state[5][q_index]=temp_state;
      break;
    case 4:
      state_cacheA=state[4][index_cacheA_index];
      state_cacheC=state[0][index_cacheC_index];
      state[6][q_index]=temp_state;
      break;
    case 5:
      state_cacheA=state[5][index_cacheA_index];
      state_cacheC=state[1][index_cacheC_index];
      state[7][q_index]=temp_state;
      break;
    case 6:
      state_cacheA=state[6][index_cacheA_index];
      state_cacheC=state[2][index_cacheC_index];
      state[0][q_index]=temp_state;
      break;
    case 7:
      state_cacheA=state[7][index_cacheA_index];
      state_cacheC=state[3][index_cacheC_index];
      state[1][q_index]=temp_state;
      break;
    }

    temp_state_next=state_cacheA^ twiddle(state_cacheB, state_cacheC);


    q_subarr=p_subarr;
    q_index=p_index;

    if(p_wrap_next){
      p_next=0;
    } else {
      p_next=p+1;
    }

    p_wrap_next= (p==n-2);

    if(index_cacheA_wrap_next){
      index_cacheA_next=0;
    } else {
      index_cacheA_next++;
    }
    index_cacheA_wrap_next= (index_cacheA==n-2);


    if(index_cacheC_wrap_next){
      index_cacheC_next=0;
    } else {
      index_cacheC_next++;
    }
    index_cacheC_wrap_next= (index_cacheC==n-2);

    x ^= (x >> MT_u);
    x ^= (x << MT_s) & MT_b;
    x ^= (x << MT_t) & MT_c;
    *out_stream = x ^ (x >> MT_l);
  }
}

void lite_to_stream(ap_uint<9> inform, ap_uint<18> *inform1, ap_uint<18> *inform2,
		ap_uint<18> *inform3, ap_uint<18> *inform4, ap_uint<18> *inform5,
		ap_uint<18> *inform6, ap_uint<18> *inform7, ap_uint<18> *inform8,
		ap_uint<18> *inform9, ap_uint<18> *inform10, ap_uint<18> *inform11){
#pragma HLS INTERFACE s_axilite port=inform
#pragma HLS INTERFACE axis register both port=inform1
#pragma HLS INTERFACE axis register both port=inform2
#pragma HLS INTERFACE axis register both port=inform3
#pragma HLS INTERFACE axis register both port=inform4
#pragma HLS INTERFACE axis register both port=inform5
#pragma HLS INTERFACE axis register both port=inform6
#pragma HLS INTERFACE axis register both port=inform7
#pragma HLS INTERFACE axis register both port=inform8
#pragma HLS INTERFACE axis register both port=inform9
#pragma HLS INTERFACE axis register both port=inform10
#pragma HLS INTERFACE axis register both port=inform11

	ap_uint<4> beta, betaV ;
	unsigned int arms, vacant, occupied ;
	ap_uint<1> b1, b2, b3, b4, flag ;
	float r, P1, P2, P3, P4 ;
	static float X1, X2, X3, X4 ;
	static float T1, T2, T3, T4 ;
	ap_uint<18> inform_in ;

	ap_uint<32> out_rand ;
	//unsigned int ui_core_rand = 0 ;

	betaV = inform.range(3,0) ;
	beta = inform.range(7,4) ;

	arms = beta[0] + beta[1] + beta[2] + beta[3] ;
	vacant = betaV[0] + betaV[1] + betaV[2] + betaV[3] ;

	occupied = arms - vacant ;

	flag = (occupied <= 2) ;

	if(inform[8]==0)
	{
		X1 = 1 ;
		X2 = 1 ;
		X3 = 1 ;
		X4 = 1 ;

		T1 = 1 ;
		T2 = 1 ;
		T3 = 1 ;
		T4 = 1 ;

		mtwist_core(true, 0x42874, 1, &out_rand) ;
	}
	else
	{
		if(flag==1)		// occupied <= K
		{
			T1 += beta[0] ;
			T2 += beta[1] ;
			T3 += beta[2] ;
			T4 += beta[3] ;

			X1 += betaV[0] ;
			X2 += betaV[1] ;
			X3 += betaV[2] ;
			X4 += betaV[3] ;
		}

		mtwist_core(false, 0x42874, 1, &out_rand);
		//ui_core_rand = (unsigned int) out_rand;
		r = (float)out_rand/UINT_MAX ;

		P1 = (X1/T1) ;
		P2 = (X2/T2) ;
		P3 = (X3/T3) ;
		P4 = (X4/T4) ;

		/*
		printf("r = %f\n",(float)r) ;
		printf("P1 = %f\n",(float)P1) ;
		printf("P2 = %f\n",(float)P2) ;
		printf("P3 = %f\n",(float)P3) ;
		printf("P4 = %f\n",(float)P4) ;
		*/

		b1 = (r > P1) ;
		b2 = (r > P2) ;
		b3 = (r > P3) ;
		b4 = (r > P4) ;
	}

	inform_in = inform ;

	inform_in = (((((((((inform_in<<1)|b4)<<1)|b3)<<1)|b2)<<1)|b1)<<1)|flag ;

	*inform1 = (inform_in<<4)|3 ;
	*inform2 = (inform_in<<4)|5 ;
	*inform3 = (inform_in<<4)|6 ;
	*inform4 = (inform_in<<4)|7 ;
	*inform5 = (inform_in<<4)|9 ;
	*inform6 = (inform_in<<4)|10 ;
	*inform7 = (inform_in<<4)|11 ;
	*inform8 = (inform_in<<4)|12 ;
	*inform9 = (inform_in<<4)|13 ;
	*inform10 = (inform_in<<4)|14 ;
	*inform11 = (inform_in<<4)|15 ;
}
