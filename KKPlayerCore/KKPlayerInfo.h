///一些对外的数据定义
#ifndef KKPlayerInfo_H_
#define KKPlayerInfo_H_
typedef struct AVCACHE_INFO
{
	int VideoSize;
	int AudioSize;
	int MaxTime;
}AVCACHE_INFO;
typedef struct AVCODE_INFO
{
  ///解码器名称
  char codecname[32];
  //平均码率
  int bitrate;
  int framerate;
  int sample_rate;
  int channels;//声 道 数
}AVCODE_INFO;
//获取播放器信息
typedef struct MEDIA_INFO 
{
	//分辨率
	char              AVRes[1024];
	//编码信息
	AVCODE_INFO       videoinfo;
	AVCODE_INFO       audioinfo;
	const char*       AvFile;
	int               FileSize;
	int               CurTime;
	int               TotalTime;//总时长
	int               serial;
    int               serial1;
	bool              Open;
	int               KKState;
    short             SegId;
	char              SpeedInfo[1024];     ///下载速度
	AVCACHE_INFO      CacheInfo;
}MEDIA_INFO ;

///视频图像信息
typedef struct kkAVPicInfo
{
    unsigned char *data[8];
    int      linesize[8];
	int      picformat;
	int      width;
	int      height;

}kkAVPicInfo;

///用于替换URL
typedef int (*fpKKPlayerGetUrl)(char* url,char** outurlstr);
#endif