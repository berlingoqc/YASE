#ifndef LOGGER_H
#define LOGGER_H

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRINGS(STRING) #STRING,


#include <mutex>
#include <vector>


#define FOREACH_LOGLEVEL(LOGLEVEL) LOGLEVEL(LERROR) LOGLEVEL(LINFO) LOGLEVEL(LWARNING)

enum LOGLEVEL_ENUM { FOREACH_LOGLEVEL(GENERATE_ENUM) };

static const char* LOGLEVEL_STRING[] = {
		FOREACH_LOGLEVEL(GENERATE_STRINGS)
};


class Logger
{
	std::mutex						mutex;
	std::vector<std::string>		logs;

public:
	void Append(LOGLEVEL_ENUM ll, const char* file, int line, const char* msg)
	{
		char buf[150];
		sprintf(buf, "[%s][%s:%d] %s", LOGLEVEL_STRING[ll], file, line, msg);
		std::unique_lock<std::mutex> lk(mutex);
		logs.emplace_back(buf);
		lk.unlock();
	}

	std::vector<std::string> Consume()
	{
		std::unique_lock<std::mutex> lk(mutex);
		std::vector <std::string> v = logs;
		logs.clear();
		return v;
	}
};


inline Logger logger;


#define YASE_LOG_INFO(msg,...) { \
	logger.Append(LINFO,__FILENAME__,__LINE__,msg);}
#define YASE_LOG_WARNING(msg,...) { \
	logger.Append(LWARNING,__FILENAME__,__LINE__,msg);}
#define YASE_LOG_ERROR(msg,...) { \
	logger.Append(LERROR,__FILENAME__,__LINE__,msg);}

#endif //LOG_H