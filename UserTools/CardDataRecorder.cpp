#include "CardDataRecorder.h"

CardDataRecorder::CardDataRecorder():Tool(){}


bool CardDataRecorder::Initialise(std::string configfile, DataModel &data){
  
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();
  
  m_data= &data;
  
  m_variables.Get("TTreeEventCap",m_TTreeEventCap); 
  m_variables.Get("TFileTTreeCap",m_TFileTTreeCap);
  m_variables.Get("OutputPath",OutputPath);

  m_filepart=0;  
  std::stringstream tmp;
  tmp<<OutputPath<<"DataR"<<m_data->RunNumber<<"S"<<m_data->SubRunNumber;
  OutFile=tmp.str();
  tmp<<"p0.root";
  TFile file(tmp.str().c_str(),"RECREATE","",1);
  file.Write();
  file.Close();
  
  //std::cout<< "outfile = "<<OutFile<<std::endl;
  
  Isend = new zmq::socket_t(*(m_data->context), ZMQ_PUSH);
  Isend->bind("inproc://RootWriter");
  
  args=new card_root_thread_args(OutFile,m_data->context,m_TFileTTreeCap, m_filepart);
  //  //pthread_create (&thread[0], NULL, CardDataRecorder::FlowManager, args);
  // pthread_create (&thread[0], NULL, CardDataRecorder::RootWriter, args);
  
  h1 = new TThread("h1", CardDataRecorder::RootWriter, args);
  h1->Run();
  
  m_treenum=0;
  m_data->NumEvents=0;
  //std::cout<<"i d1 "<<std::endl;
  // std::cout<<m_data->carddata.at(0)->channels<<std::endl;

  m_card.PMTID=new int[4];
  //std::cout<<"i d2"<<std::endl;  
m_card.Data=new uint16_t[160000];
//std::cout<<"i d3"<<std::endl;
  TTree *tree = new TTree("PMTData0","PMTData0");
  //std::cout<<"i d4"<<std::endl;
  tree->Branch("LastSync",&(m_card.LastSync),"LastSync/I");
  tree->Branch("SequenceID",&(m_card.SequenceID),"SequenceID/I");
  tree->Branch("StartTime",&(m_card.StartTime),"StartTime/I");
  tree->Branch("CardID",&(m_card.CardID),"CardID/I");
  tree->Branch("Channels",&(m_card.channels),"Channels/I");
  tree->Branch("PMTID",m_card.PMTID,"PMTID[Channels]/I");
  tree->Branch("BufferSize",&(m_card.buffersize),"BufferSize/I");
  tree->Branch("FullBufferSize",&(m_card.fullbuffsize),"FullBufferSize/I");
  tree->Branch("Data",m_card.Data,"Data[FullBufferSize]/s");
  //std::cout<<"i d5"<<std::endl;
  m_data->AddTTree("PMTData",tree);
  //std::cout<<"i d6"<<std::endl;
  return true;
}


bool CardDataRecorder::Execute(){
  //std::cout<<"Debug 1 i="<<m_data->NumEvents<<std::endl;
  if(m_data->triggered){

    //    boost::progress_timer t;

  m_data->NumEvents++;
  
  TTree *tree=m_data->GetTTree("PMTData");
  //std::cout<<"Debug 2"<<std::endl;

  if (tree->GetEntriesFast()>m_TTreeEventCap){
    //std::cout<<"Debug 3"<<std::endl;
    //    root_thread_args *args=new root_thread_args(OutFile,tree);
    //std::cout<<"Debug 4 "<<tree<<std::endl;
    
    
    zmq::message_t message(512);
    std::stringstream TTreepointer;
    TTreepointer<<"TTree "<<tree;
    
    //std::cout<<"sending "<<TTreepointer.str()<<std::endl;
    
    snprintf ((char *) message.data(), 512 , "%s" ,TTreepointer.str().c_str()) ;
    Isend->send(message);
    
    //std::cout<<"sent "<<std::endl;
    //    pthread_create (&thread[0], NULL, CardDataRecorder::WriteOut, args
    //std::cout<<"Debug 5"<<std::endl;
    m_treenum++;
    std::stringstream tmp;
    tmp<<"PMTData"<<m_treenum;
    //std::cout<<"Debug 6 m_tmp = "<<tmp.str()<<std::endl;
    tree=new TTree(tmp.str().c_str(),tmp.str().c_str());
    //std::cout<<"Debug 7"<<std::endl;
    m_data->AddTTree("PMTData",tree);
    //std::cout<<"Debug 8"<<std::endl;
    tree->Branch("LastSync",&(m_card.LastSync),"LastSync/I");
    tree->Branch("SequenceID",&(m_card.SequenceID),"SequenceID/I");
    tree->Branch("StartTime",&(m_card.StartTime),"StartTime/I");
    tree->Branch("CardID",&(m_card.CardID),"CardID/I");
    tree->Branch("Channels",&(m_card.channels),"Channels/I");
    tree->Branch("PMTID",(m_card.PMTID),"PMTID[Channels]/I");
    tree->Branch("BufferSize",&(m_card.buffersize),"BufferSize/I");
    tree->Branch("FullBufferSize",&(m_card.fullbuffsize),"FullBufferSize/I");
    tree->Branch("Data",(m_card.Data),"Data[FullBufferSize]/s");
    //std::cout<<"Debug 9"<<std::endl;
  }


  //std::cout<<"Debug 10 "<<m_data->carddata.size()<<std::endl;
  for(int i=0;i<m_data->carddata.size();i++){
    //std::cout<<"Debug 11"<<std::endl;
    m_card.LastSync=m_data->carddata.at(i)->LastSync;
    m_card.SequenceID=m_data->carddata.at(i)->SequenceID;
    m_card.StartTime=m_data->carddata.at(i)->StartTime;
    m_card.CardID=m_data->carddata.at(i)->CardID;
    m_card.channels=m_data->carddata.at(i)->channels;
    m_card.buffersize=m_data->carddata.at(i)->buffersize;
    m_card.fullbuffsize=m_data->carddata.at(i)->fullbuffsize;
    //std::cout<<"Debug 12"<<std::endl;
    for(int j=0;j<m_card.channels;j++){
      //std::cout<<"Debug 13"<<std::endl;

     (m_card.PMTID)[j]=(m_data->carddata.at(i)->PMTID)[j]; // these pointers have changed hence the crash
     //std::cout<<"Debug 14"<<std::endl;
      
    }	
    //std::cout<<"Debug 15"<<std::endl;
    
    for(int j=0;j<m_card.fullbuffsize;j++){
      //std::cout<<"Debug 16"<<std::endl;
      
      (m_card.Data[j])=(m_data->carddata.at(i)->Data)[j];
      //  std::cout<<m_data->carddata.at(i)->Data[j]<<" Debug 17 "<<m_card.Data[j]<<std::endl;

    }
    
    //std::cout<<"Debug 18 i= "<<m_data->NumEvents<<std::endl;
     tree->Fill();
     //std::cout<<"Debug 19"<<std::endl;
  }    
  
  //std::cout<<"Debug 20"<<std::endl;
  
  
  
  }

  return true;
}


bool CardDataRecorder::Finalise(){

 

  zmq::message_t message(256);
  std::string send="Quit 0x00";

  //std::cout<<"sending "<<send<<std::endl;

  snprintf ((char *) message.data(), 256 , "%s" ,send.c_str()) ;
  Isend->send(message);

  //TThread::Ps();
  h1->Join();
  //  (void) pthread_join(thread[0], NULL);
  m_filepart++;  
  std::stringstream tmp;
  tmp<<OutFile<<"p"<<m_filepart<<".root";
  TFile file(tmp.str().c_str(),"UPDATE","",1);
  
  TTree *tree=m_data->GetTTree("PMTData");
  tree->Write();
  file.Write();
  file.Close();
  
  // sleep(5);

  m_data->DeleteTTree("PMTData");
  tree=0;

  //  Isend->close();

  // too delete properly 

  delete h1;
  h1=0;

  delete Isend;
  Isend=0;
  
  delete args;
  args=0;
  
  return true;
}

/*
void* CardDataRecorder::WriteOut(void* arg){
  //  std::cout<<"Debug a1"<<std::endl;
  root_thread_args* args= static_cast<root_thread_args*>(arg);
  //std::cout<<"Debug a2"<<std::endl;

  std::cout<<args->OutFile<<std::endl;
  //  std::cout<<"Debug a3"<<std::endl;

  TFile file(args->OutFile.c_str(),"UPDATE");
  //  std::cout<<"Debug a4 "<<args->tree<<std::endl;

  args->tree->Write();
  //std::cout<<"Debug a5"<<std::endl;

  file.Write();
  //std::cout<<"Debug a6"<<std::endl;

  file.Close();
  //std::cout<<"Debug a7"<<std::endl;

  delete args->tree;
  //std::cout<<"Debug a8"<<std::endl;

  args->tree=0;
  std::cout<<"Debug a9"<<std::endl;
  
  return (NULL);
  
}

*/


void* CardDataRecorder::RootWriter(void* arg){
  
  card_root_thread_args* args= static_cast<card_root_thread_args*>(arg);
 
  zmq::socket_t Ireceive (*(args->context), ZMQ_PULL);
  Ireceive.connect("inproc://RootWriter");

  bool running=true;
  *(args->filepart)=0;
  int treenum=0;
   

  while (running){
    //std::cout<<"T  Debug 1"<<std::endl;
    zmq::message_t comm;
    //std::cout<<"T  Debug 2"<<std::endl;
    Ireceive.recv(&comm);
    
    std::istringstream iss(static_cast<char*>(comm.data()));
    std::string arg1="";
    long long unsigned int arg2;    
    //lon arg2="";
    TTree *tree;
    
    iss>>arg1>>std::hex>>arg2;
    
    //std::cout<<"arg1 = "<<arg1<<std::endl;
    //std::cout<<"arg2 = "<<arg2<<std::endl;
    //std::cout<<"T  Debug 3"<<std::endl;
    if (arg1=="TTree"){
      //std::cout<<"T  Debug 4"<<std::endl;
      treenum++;
     
      if(treenum>args->TFileTTreeCap){
	(*(args->filepart))+=1;
	treenum=1;	
      }
      tree=reinterpret_cast<TTree *>(arg2);
      std::stringstream tmp;
      tmp<<args->OutFile<<"p"<<*(args->filepart)<<".root";
      TFile file(tmp.str().c_str(),"UPDATE","",1);
      //std::cout<<"tree = "<<tree<<std::endl;
      tree->Write();
      //std::cout<<"T  Debug 5"<<std::endl;
      file.Write();
      //std::cout<<"T Debug 6"<<std::endl;
      file.Close();
      
      //std::cout<<"T  Debug 7"<<std::endl;
      //sleep(5);
      //std::cout<<"T  Debug 8"<<std::endl;
      delete tree;
      //std::cout<<"T  Debug 9"<<std::endl;
      tree=0;
      //std::cout<<"T  Debug 10"<<std::endl;
     
    }
    
    else if(arg1=="Quit"){
      running=false;
      //std::cout<<"T  Debug 12"<<std::endl;
    }
    //std::cout<<"T  Debug 13"<<std::endl;
  }
  //std::cout<<"T  Debug 14"<<std::endl;

  //std::cout<<"exiting thread"<<std::endl;
  //pthread_exit(NULL);
  //std::cout<<"exit"<<std::endl;
}