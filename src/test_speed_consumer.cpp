/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015 Regents of the University of California.
 *
 * @author Lijing Wang phdloli@ucla.edu
 */

#include "test_speed_consumer.hpp"
#include <boost/thread/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <thread>
#include <deque>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions

#define VIDEO_RATE 7
#define AUDIO_RATE 6

  struct Consumer_Need
  {
    int end;
    int rate;
    std::string name;
    ConsumerCallback *cb;
    Consumer* sampleConsumer;
  };

  void
  consumer_setup(Consumer_Need *con)
  {
    if(con->name == "video")
    {
      con->sampleConsumer->setContextOption(CONTENT_RETRIEVED, 
                          (ConsumerContentCallback)bind(&ConsumerCallback::processTestPayload, con->cb, _1, _2, _3));

    }else
    {
      con->sampleConsumer->setContextOption(CONTENT_RETRIEVED, 
                          (ConsumerContentCallback)bind(&ConsumerCallback::processTestPayloadAudio, con->cb, _1, _2, _3));
    }
        
    con->sampleConsumer->setContextOption(MUST_BE_FRESH_S, true);
    con->sampleConsumer->setContextOption(INTEREST_LIFETIME, 500);

    con->sampleConsumer->setContextOption(INTEREST_RETX,16); //Retransmitted Attempted Time.

    con->sampleConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
                              (ConsumerInterestCallback)bind(&ConsumerCallback::processLeavingInterest, con->cb, _1, _2));
    con->sampleConsumer->setContextOption(INTEREST_RETRANSMIT, 
                              (ConsumerInterestCallback)bind(&ConsumerCallback::onRetx, con->cb, _1, _2));
    con->sampleConsumer->setContextOption(INTEREST_EXPIRED, 
                              (ConsumerInterestCallback)bind(&ConsumerCallback::onExpr, con->cb, _1, _2));
    con->sampleConsumer->setContextOption(DATA_ENTER_CNTX, 
                              (ConsumerDataCallback)bind(&ConsumerCallback::processData, con->cb, _1, _2));
  }

  time_t time_start;
  ConsumerCallback cb_consumer(VIDEO_RATE, AUDIO_RATE); 

  static void sig_int(int num)
  {
    time_t time_end = std::time(0);
    double seconds = difftime(time_end, time_start);
    std::cout << "  " << std::endl;
    std::cout << "｡:.ﾟヽ(*´∀`)ﾉﾟ. Test Result ヽ(✿ﾟ▽ﾟ)ノ  " << std::endl;
    std::cout << "  " << std::endl;
    std::cout << "-------- Throughput --------  " << std::endl;
    std::cout << "Video Bytes: " << std::dec << cb_consumer.payload_v << "  PayLoad_Throughput: " << cb_consumer.payload_v/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Audio Bytes: " << cb_consumer.payload_a << "  PayLoad_Throughput: " << cb_consumer.payload_a/seconds << " Bytes/Seconds" <<std::endl;
    std::cout << "Total Bytes: " << cb_consumer.payload_v + cb_consumer.payload_a << "  PayLoad_Throughput: " << (cb_consumer.payload_v + cb_consumer.payload_a)/seconds << " Bytes/Seconds" << std::endl;

    std::cout << "Video Frames: " << std::dec << cb_consumer.frame_cnt_v << "  Frame_Throughput: " << cb_consumer.frame_cnt_v/seconds << " /Seconds" <<std::endl;
    std::cout << "Audio Frames: " << std::dec << cb_consumer.frame_cnt_a << "  Frame_Throughput: " << cb_consumer.frame_cnt_a/seconds << " /Seconds" <<std::endl;
    std::cout << "Total Frames: " << cb_consumer.frame_cnt_v + cb_consumer.frame_cnt_a << "  Frame_Throughput: " << (cb_consumer.frame_cnt_v + cb_consumer.frame_cnt_a)/seconds << " /Seconds" << std::endl;

    std::cout << "  " << std::endl;
    std::cout << "-------- Interest --------  " << std::endl;
    std::cout << "Interest_Send: " << cb_consumer.interest_s << "  Interest_Send_Speed: " <<  cb_consumer.interest_s/seconds << " /Seconds" <<  std::endl;
    std::cout << "Interest_Receive: " << cb_consumer.interest_r << "  Interest_Receive_Speed: " <<  cb_consumer.interest_r /seconds << " /Seconds" <<  std::endl;
    std::cout << "Interest_Retransmit: " << cb_consumer.interest_retx << " Interest_Retransfer_Speed: " << cb_consumer.interest_retx/seconds << " /Seconds" << std::endl;
    std::cout << "Interest_Expire: " << cb_consumer.interest_expr << " Interest_Retransfer_Speed: " << cb_consumer.interest_expr/seconds << " /Seconds" << std::endl;
    std::cout << "Interest_Useful Percentage: " << double(cb_consumer.interest_r)/double(cb_consumer.interest_s)*100 <<"%" << std::endl;
    std::cout << "  " << std::endl;
    std::cout << "---------- OVER ----------  " << seconds <<" seconds" << std::endl;

    return;
  }

  void consume_frame(std::string type, Consumer *sampleConsumer, int frameNumber) 
  {
    Name sampleSuffix(std::to_string(frameNumber));
    printf("%s Number %d before cosume\n", type.c_str(), frameNumber);
    sampleConsumer->consume(sampleSuffix);
    printf("%s Number %d after cosume\n", type.c_str(), frameNumber);
  }

  void consume_control(Consumer *sampleConsumer, int end)
  {
    signal(SIGINT, sig_int);
    for (int i = 0; i < end; i++)
    { 
      Name sampleSuffix(std::to_string(i));

      signal(SIGINT, sig_int);
      printf("Number %d before cosume End:%d\n",i, end);
      sampleConsumer->consume(sampleSuffix);
      printf("Number %d after cosume End:%d\n",i, end);

    }
    sig_int(0);
    pthread_exit(NULL);
  }

  int
  main(int argc, char** argv)
  {
    try {
  
      boost::property_tree::ptree pt;
      boost::property_tree::ini_parser::read_ini("config.ini", pt);

  		std::string prefix = pt.get<std::string>("ndntube.prefix");
      std::string filename ="";

      Name listName(Name(prefix).append("playlist"));
      Consumer* listConsumer = new Consumer(listName, SDR);
      listConsumer->setContextOption(INTEREST_LIFETIME, 50);
      listConsumer->setContextOption(MUST_BE_FRESH_S, true);
      listConsumer->setContextOption(RIGHTMOST_CHILD_S, true);
      listConsumer->setContextOption(INTEREST_LEAVE_CNTX, 
        (ConsumerInterestCallback)bind(&ConsumerCallback::processLeavingInterest, &cb_consumer, _1, _2));
      listConsumer->setContextOption(CONTENT_RETRIEVED, 
        (ConsumerContentCallback)bind(&ConsumerCallback::processList, &cb_consumer, _1, _2, _3));
      listConsumer->setContextOption(DATA_ENTER_CNTX, 
        (ConsumerDataCallback)bind(&ConsumerCallback::processData, &cb_consumer, _1, _2));
  		if(argc < 2)
      {
        listConsumer->consume(Name("all"));
        boost::unique_lock<boost::mutex> lock(cb_consumer.mut);
        while(!cb_consumer.data_ready)
        {
            cb_consumer.cond.wait(lock);
        }
  
        int numberInput = 0;
        std::cout << "Please Input the Video Number " <<std::endl;
        std::cin >> numberInput;
        while(std::cin.fail() || numberInput >= cb_consumer.list.size())
        {
          if(std::cin.fail())
            std::cout << "Please Input a NUMBER "  <<std::endl;
          else
            std::cout << "Please Input a number <= " << cb_consumer.list.size() <<std::endl;
          std::cin >> numberInput;
        }
  
        filename = cb_consumer.list[numberInput];
  
      }else
        filename = argv[1];


      std::cout << "Asking For " << filename << std::endl;
      std::cout << "Only for speed test" <<std::endl;

      time_start = std::time(0);



      Consumer_Need audioData;
      audioData.name = "audio";
      audioData.end = 13838 / 10; 
      audioData.cb = &cb_consumer;
      audioData.rate = AUDIO_RATE; //23;
      Name audioName = Name(prefix).append(filename).append("audio").append("content");
//      std::thread audioThread(consume_control, audioData.sampleConsumer, audioData.end);
      
//      tp.schedule(boost::bind(consume_control, audioData.sampleConsumer, audioData.end));

      Consumer_Need videoData;
      videoData.name = "video";
      videoData.end = 16062 / 10;
      videoData.cb = &cb_consumer;
      videoData.rate = VIDEO_RATE; //27;
      Name videoName = Name(prefix).append(filename).append("video").append("content");


      std::vector<Consumer *> audio_consumers;
      std::vector<Consumer *> video_consumers;
      std::deque<DataNode> video_q;
      std::deque<DataNode> audio_q;

      for (int j = 0; j < videoData.rate; j++) {
        videoData.sampleConsumer = new Consumer(videoName, RDR);
        consumer_setup(&videoData);
        video_consumers.push_back(videoData.sampleConsumer);
      }

      for (int j = 0; j < audioData.rate; j++) {
        audioData.sampleConsumer = new Consumer(audioName, RDR);
        consumer_setup(&audioData);
        audio_consumers.push_back(audioData.sampleConsumer);
      } 

//      consume_control(videoData.sampleConsumer, videoData.end);
//      std::thread videoThread(consume_control, videoData.sampleConsumer, videoData.end);
//      tp.schedule(boost::bind(consume_control, videoData.sampleConsumer, videoData.end));
      int audio_counter = 0;
      int video_counter = 0;
      uint64_t audio_size = 0;
      uint64_t video_size = 0;

      pool tp_video(videoData.rate);
      pool tp_audio(audioData.rate);

      signal(SIGINT, sig_int);

      while (audio_counter < audioData.end || video_counter < videoData.end) {

        cb_consumer.buffers_a.clear();
        cb_consumer.buffers_v.clear();

        int init_video_counter = video_counter;
        int init_audio_counter = audio_counter;

        for (int j = 0; video_counter < videoData.end && j < videoData.rate; j++) {
          cb_consumer.buffers_v[j] = NULL;
          tp_video.schedule(boost::bind(consume_frame, "video", video_consumers[j], video_counter));
          video_counter++;
        }

        for (int j = 0; audio_counter < audioData.end && j < audioData.rate; j++) {
          cb_consumer.buffers_a[j] = NULL;
          tp_audio.schedule(boost::bind(consume_frame, "audio", audio_consumers[j], audio_counter));
          audio_counter++;
        } 

        tp_video.wait();
        tp_audio.wait();
        
        for (int j = 0; init_video_counter < videoData.end && j < videoData.rate; j++) {
          if (cb_consumer.buffers_v[j]) {
            DataNode dataNode;
            uint64_t bufferSize = cb_consumer.buffers_v[j]->size();
            uint8_t* buffer = cb_consumer.buffers_v[j]->buf();
            uint8_t* bufferTmp = new uint8_t[bufferSize];
            memcpy (bufferTmp, buffer, bufferSize);
            dataNode.length = bufferSize;
            dataNode.data = (uint8_t *)bufferTmp;
            video_q.push_back(dataNode);
            video_size += dataNode.length;
          }
            
          init_video_counter++;
        }

        for (int j = 0; init_audio_counter < audioData.end && j < audioData.rate; j++) {
          if (cb_consumer.buffers_a[j]) {
            DataNode dataNode;
            uint64_t bufferSize = cb_consumer.buffers_a[j]->size();
            uint8_t* buffer = cb_consumer.buffers_a[j]->buf();
            uint8_t* bufferTmp = new uint8_t[bufferSize];
            memcpy (bufferTmp, buffer, bufferSize);
            dataNode.length = bufferSize;
            dataNode.data = (uint8_t *)bufferTmp;
            audio_q.push_back(dataNode);
            audio_size += dataNode.length;
          }

          init_audio_counter++;
        }

      }

      std::cout << "video_counter " << video_counter << std::endl;
      std::cout << "video_q size " << video_q.size() <<  " buffer_size " << video_size << std::endl;
      std::cout << "audio_counter " << audio_counter << std::endl;
      std::cout << "audio_q size " << audio_q.size() <<  " buffer_size " << audio_size << std::endl;

//      tp.wait();
//      audioThread.join();
//      videoThread.join();
      sig_int(0);

//      sleep(3000); // because consume() is non-blocking
      
    }
    catch(std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
    return 0;
  }
} // namespace ndn

int
main(int argc, char** argv)
{
  return ndn::main(argc, argv);
}
