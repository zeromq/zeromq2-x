#ifndef __LOGGER_HPP_INCLUDED__
#define __LOGGER_HPP_INCLUDED__

#include <iostream>

#define LOGI() (std::cerr << "INF: ") 
#define LOGD() (std::cerr << "DBG: ") 
#define LOGE() (std::cerr << "ERR: ")

#define LOGF() \
	(std::cerr << "FAL: [" << __FILE__ << ":" << __LINE__ << "] ")

#define LOG_ENDL() std::endl 

#endif // __LOGGER_HPP_INCLUDED__
