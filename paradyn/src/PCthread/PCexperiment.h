/*
 * Copyright (c) 1993, 1994 Barton P. Miller, Jeff Hollingsworth,
 *     Bruce Irvin, Jon Cargille, Krishna Kunchithapadam, Karen
 *     Karavanic, Tia Newhall, Mark Callaghan.  All rights reserved.
 * 
 * This software is furnished under the condition that it may not be
 * provided or otherwise made available to, or used by, any other
 * person, except as provided for by the terms of applicable license
 * agreements.  No title to or ownership of the software is hereby
 * transferred.  The name of the principals may not be used in any
 * advertising or publicity related to this software without specific,
 * written prior authorization.  Any use of this software must include
 * the above copyright notice.
 *
 */

/*
 * PCexperiment.h
 *
 * experiment class
 *
 * $Log: PCexperiment.h,v $
 * Revision 1.4  1996/05/08 07:35:09  karavan
 * Changed enable data calls to be fully asynchronous within the performance consultant.
 *
 * some changes to cost handling, with additional limit on number of outstanding enable requests.
 *
 * Revision 1.3  1996/05/02 19:46:31  karavan
 * changed predicted data cost to be fully asynchronous within the pc.
 *
 * added predicted cost server which caches predicted cost values, minimizing
 * the number of calls to the data manager.
 *
 * added new batch version of ui->DAGconfigNode
 *
 * added hysteresis factor to cost threshold
 *
 * eliminated calls to dm->enable wherever possible
 *
 * general cleanup
 *
 * Revision 1.2  1996/04/30 06:26:47  karavan
 * change PC pause function so cost-related metric instances aren't disabled
 * if another phase is running.
 *
 * fixed bug in search node activation code.
 *
 * added change to treat activeProcesses metric differently in all PCmetrics
 * in which it is used; checks for refinement along process hierarchy and
 * if there is one, uses value "1" instead of enabling activeProcesses metric.
 *
 * changed costTracker:  we now use min of active Processes and number of
 * cpus, instead of just number of cpus; also now we average only across
 * time intervals rather than cumulative average.
 *
 * Revision 1.1  1996/02/02 02:07:25  karavan
 * A baby Performance Consultant is born!
 *
 */

#ifndef PCEXPER_H
#define PCEXPER_H

#include "PCintern.h"
#include "PCwhy.h"
#include "PCmetricInst.h"
#include "PCdata.h"

/******************************************************
Experiment 
========== 
Each node of the Search History Graph represents
a single experiment.  An experiment tests the value of a PCmetric for a
particular focus over some time interval.

*******************************************************/

class filter;
class PCsearch;

class experiment : public dataSubscriber
{
  friend ostream& operator <<(ostream &os, experiment& ex);
 public:
  experiment(hypothesis *whyowhy, focus whereowhere, bool persist, 
	     searchHistoryNode *papa, PCsearch *srch, bool amFlag, bool *err);
  ~experiment();
  //
  // subscribe/unsubscribe to flow of data which keeps 
  // experiment running
  bool start();
  void halt();
  // 
  // experiment status methods
  int getCurrentStatus() 
    {if (status) return 1; else return 0;}
  testResult getCurrentConclusion (){return currentConclusion;}
  timeStamp getTimeTrueFalse () {return timeTrueFalse;}
  sampleValue getCurrentValue () {return currentValue;}
  timeStamp getStartTime () {return startTime;}
  timeStamp getEndTime () {return endTime;}
  float getEstimatedCost() {return estimatedCost;}
  //
  // this call invoked by PCmetricInst to notify experiment of new 
  // values  
  void newData(PCmetDataID, float, double, double, float);
  void enableReply (unsigned token1, unsigned token2, unsigned token3,
		    bool successful);
  //
  // return cost value either in response to initialization async request
  // or when a resource update has resulted in a change to the specific 
  // metric-focus pairs being used by this experiment.
  void updateEstimatedCost(float costDiff); 
 private:
  // a true/false guess has held for the appropriate minimum interval
  void drawConclusion(testResult newConclusion);

  hypothesis *why;
  focus where;
  PCmetInstHandle pcmih;
  bool persistent;    // true means keep testing even if false
  PCsearch *mamaSearch;
  searchHistoryNode *papaNode;
  float estimatedCost;
  //
  // ## current test status information ##
  //
  // status is active (true) if currently subscribed to PCmetric data; 
  // inactive (false) otherwise
  bool status;    
  // true/false/unknown.  true/false conclusion can only be reached after
  // the correct minimum interval worth of data has been observed.
  testResult currentConclusion;      
  // unlike conclusion, guess will always be true or false for an active 
  // experiment which has received at least one piece of data.  A guess 
  // which remains the same for the proper interval results in a conclusion.
  testResult currentGuess;           // not long enough yet to say fer sure
  // how long has this guess held? (sum of intervals used in guess, not 
  // necessarily consecutive)
  timeStamp timeTrueFalse;      
  // this is value of PC metric minus threshold; only the sign has 
  // meaning, magnitudes mean varying things across different experiments.
  sampleValue currentValue;
  // always contains the correct hysteresis parameter to use for next 
  // evaluation.  1-hysparam if been true; else 1+hysparam
  float hysConstant;     
  // time of first data received
  timeStamp startTime;      
  // time of most recent data received
  timeStamp endTime;        
  // true if minObs time has passed
  bool minObservationFlag;  
};

ostream& operator <<(ostream &os, experiment& ex);
        
#endif



