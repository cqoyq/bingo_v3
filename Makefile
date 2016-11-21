INCLUDE = -I$(BOOST_INCLUDE_ROOT) \
          -I$(MYSQL_INCLUDE_ROOT) \
          -I./

LIBS =  -lamqpcpp \
		-lpthread \
	   -lmysqlclient_r \
	   -lsqlite3 \
	   -lrt \
	   -lboost_random \
	   -lboost_regex \
       -lboost_chrono \
	   -lboost_timer \
	   -lboost_system \
	   -lboost_thread \
	   -lboost_context \
	   -lboost_coroutine \
       -lboost_unit_test_framework
	  
LIBPATH = -L$(BOOST_LIB_ROOT) \
		   -L$(MYSQL_LIB_ROOT)

OBJS =  main.o \
		bingo/string.o \
		bingo/error_what.o \
		bingo/config/node.o \
		bingo/config/xml/xml_parser.o \
		bingo/config/json/json_parser.o \
		bingo/database/db_factory.o \
		bingo/database/db_field.o \
		bingo/database/db_row.o \
		bingo/database/db_result.o \
		bingo/database/db_connector.o \
		bingo/database/config/cfg.o \
		bingo/database/mysql/mysql_visitor.o \
		bingo/database/mysql/mysql_connector.o \
		bingo/database/mysql/mysql_pool.o \
		bingo/database/mysql/mysql_timer.o \
		bingo/log/log_factory.o \
		bingo/log/log_level.o \
		bingo/log/logger.o \
		bingo/log/config/local_cfg.o \
		bingo/log/config/rbmq_cfg.o \
		bingo/log/handlers/file_logger_every_day.o \
		bingo/log/handlers/file_logger_every_hour.o \
		bingo/log/handlers/file_logger_no_postfix.o \
		bingo/log/handlers/local_log_visitor.o \
		bingo/log/handlers/rbmq_log_visitor.o \
		bingo/RabbitMQ/rabbitmq_factory.o \
		bingo/RabbitMQ/visitor/asiohandler.o \
		bingo/RabbitMQ/config/rb_receiver_cfg.o \
		bingo/RabbitMQ/config/rb_sendor_cfg.o \
		bingo/RabbitMQ/handlers/single_sendor.o \
		bingo/RabbitMQ/handlers/single_receiver.o \
		bingo/TCP/config/sendor_cfg.o \
		bingo/TCP/config/receiver_cfg.o
		
CPPS = bingo/string.cpp \
		bingo/error_what.cpp \
		bingo/config/node.cpp \
		bingo/config/xml/xml_parser.cpp \
		bingo/config/json/json_parser.cpp \
		bingo/database/db_factory.cpp \
		bingo/database/db_field.cpp \
		bingo/database/db_row.cpp \
		bingo/database/db_result.cpp \
		bingo/database/db_connector.cpp \
		bingo/database/config/cfg.cpp \
		bingo/database/mysql/mysql_visitor.cpp \
		bingo/database/mysql/mysql_connector.cpp \
		bingo/database/mysql/mysql_pool.cpp \
		bingo/database/mysql/mysql_timer.cpp \
		bingo/log/log_factory.cpp \
		bingo/log/log_level.cpp \
		bingo/log/logger.cpp \
		bingo/log/config/local_cfg.cpp \
		bingo/log/config/rbmq_cfg.cpp \
		bingo/log/handlers/file_logger_every_day.cpp \
		bingo/log/handlers/file_logger_every_hour.cpp \
		bingo/log/handlers/file_logger_no_postfix.cpp \
		bingo/log/handlers/local_log_visitor.cpp \
		bingo/log/handlers/rbmq_log_visitor.cpp \
		bingo/RabbitMQ/rabbitmq_factory.cpp \
		bingo/RabbitMQ/visitor/asiohandler.cpp \
		bingo/RabbitMQ/config/rb_receiver_cfg.cpp \
		bingo/RabbitMQ/config/rb_sendor_cfg.cpp \
		bingo/RabbitMQ/handlers/single_sendor.cpp \
		bingo/RabbitMQ/handlers/single_receiver.cpp \
		bingo/TCP/config/sendor_cfg.cpp \
		bingo/TCP/config/receiver_cfg.cpp
		
ifeq ($(ShowDebug),y)
DEBUGS = -DBINGO_TCP_SERVER_DEBUG \
		 -DBINGO_TCP_CLIENT_DEBUG \
		 -DBINGO_THREAD_TASK_DEBUG \
		 -DBINGO_PROCESS_SHARED_MEMORY_DEBUG \
		 -DBINGO_MYSQL_DEBUG
else
DEBUGS =
endif

ifeq ($(findstring Test_Debug,$(ConfigName)),Test_Debug)
	CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 $(DEBUGS) $(INCLUDE) 
	TARGET = mytest
else ifeq ($(findstring Lib_Debug,$(ConfigName)),Lib_Debug)
	CXXFLAGS =	-O2 -g -fPIC  -shared -std=c++11 $(INCLUDE)
	TARGET = libbingo_v3_d.so
else ifeq ($(findstring Lib_Release,$(ConfigName)),Lib_Release)
	CXXFLAGS =	-O2 -fPIC  -shared -std=c++11 $(INCLUDE)
	TARGET = libbingo_v3.so
endif

ifeq ($(findstring Test_,$(ConfigName)),Test_)
$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(LIBPATH);

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
else ifeq ($(findstring Lib_,$(ConfigName)),Lib_)
all:
	$(CXX)  $(CXXFLAGS) -o $(TARGET) $(CPPS) $(LIBS) $(LIBPATH);
	rm -fR $(MY_LIB_ROOT_v3)/bingo/include/bingo
	cp $(TARGET) $(MY_LIB_ROOT_v3);
	cp -fR bingo $(MY_LIB_ROOT_v3)/bingo/include/bingo;
	find $(MY_LIB_ROOT_v3)/bingo/include/bingo/ -regextype "posix-egrep" -regex '.*\.(o|cpp)' -exec rm -rf {} \;

clean:
	rm -f $(TARGET)
endif


