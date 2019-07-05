#pragma once


#define RECEIVER_THREAD_IDX 0
#define SENDER_THREAD_IDX 1
#define THREAD_NUM 2

DECLARE_LOG_CATEGORY_EXTERN(LogMessageProtocolsModule,Log,All)

#define ERR_CAT(category,message,...)\
UE_LOG(category,Error,TEXT(message),__VA_ARGS__)

#define LOG_ERR(message,...)\
ERR_CAT(LogMessageProtocolsModule,message,__VA_ARGS__)

#define WARN_CAT(category,message,...)\
UE_LOG(category,Warning,TEXT(message),__VA_ARGS__)

#define LOG_WARN(message,...)\
WARN_CAT(LogMessageProtocolsModule,message,__VA_ARGS__)

#define LOG(message,...)\
UE_LOG(LogMessageProtocolsModule,Log,TEXT(message),__VA_ARGS__)

#if WITH_EDITOR

#define LOG_CAT(category,message,...)\
UE_LOG(category,Log,TEXT(message),__VA_ARGS__)

#define LOGGER(message,...)\
LOG_CAT(LogMessageProtocolsModule,message,__VA_ARGS__)

#else

#define LOG_CAT(category,message,...)

#define LOGGER(message,...)

#endif