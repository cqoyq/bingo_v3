/*
 * file_logger_without_postfix.h
 *
 *  Created on: 2016-8-31
 *      Author: root
 */

#ifndef BINGO_LOG_HANDLERS_FILE_LOGGER_NO_POSTFIX_H_
#define BINGO_LOG_HANDLERS_FILE_LOGGER_NO_POSTFIX_H_

#include "file_logger_every_day.h"

namespace bingo {
      namespace log {
            namespace handlers {

                  class file_logger_no_postfix : public file_logger_every_day {
                  public:
                        file_logger_no_postfix(const char* directory, const char* filename);
                        virtual ~file_logger_no_postfix();

                  protected:
                        void get_filename_postfix(string& filename);
                  };

            }
      }
}

#endif /* BINGO_LOG_HANDLERS_FILE_LOGGER_NO_POSTFIX_H_ */
