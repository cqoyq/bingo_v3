############# Include path ############# 
INCLUDE = -I$(BOOST_INCLUDE_ROOT) \
	-I$(MYSQL_INCLUDE_ROOT) \
	-I./


############# Library path and lib file ############# 
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
	-lboost_date_time \
	-lboost_unit_test_framework
	  
LIBPATH = -L$(BOOST_LIB_ROOT) \
	-L$(MYSQL_LIB_ROOT)
 		   
		
############# Exe source file ############# 
OBJS =  main.o \
		bingo/string.o \
		bingo/error_what.o \
		bingo/configuration/node.o \
		bingo/configuration/xml/xml_parser.o \
		bingo/configuration/json/json_parser.o \
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
		bingo/RabbitMQ/handlers/simple_sendor.o \
		bingo/RabbitMQ/handlers/simple_receiver.o \
		bingo/RabbitMQ/handlers/work_sendor.o \
		bingo/RabbitMQ/handlers/work_receiver.o \
		bingo/RabbitMQ/handlers/publish_sendor.o \
		bingo/RabbitMQ/handlers/publish_receiver.o \
		bingo/RabbitMQ/handlers/routing_sendor.o \
		bingo/RabbitMQ/handlers/routing_receiver.o \
		bingo/TCP/config/sendor_cfg.o \
		bingo/TCP/config/receiver_cfg.o \
		test/test_string_ex.o \
		test/test_singleton.o \
		test/test_mem_guard.o \
		test/configuration/test_json.o \
		test/configuration/test_xml.o \
		test/database/test_mysql_visitor.o \
		test/database/test_mysql_pool.o \
		test/log/test_file_logger.o \
		test/thread/test_many_to_one_t1.o \
		test/thread/test_many_to_one_t2.o \
		test/thread/test_many_to_one_t3.o \
		test/thread/test_many_to_many_t1.o \
		test/thread/test_many_to_many_t2.o \
		test/thread/test_many_to_many_t3.o \
		test/thread/test_many_to_many_t4.o \
		test/tcp/test_server_t1.o \
		test/tcp/test_server_t2.o \
		test/tcp/test_server_t3.o \
		test/tcp/test_server_t4.o \
		test/tcp/test_server_t5.o \
		test/tcp/test_server_t6.o \
		test/tcp/test_server_t7.o \
		test/tcp/test_server_t8.o \
		test/tcp/test_server_t9.o \
		test/tcp/test_server_t10.o \
		test/tcp/test_server_t11.o \
		test/tcp/test_client_t1.o \
		test/tcp/test_client_t2.o \
		test/tcp/test_client_t3.o \
		test/tcp/test_client_t4.o \
		test/tcp/test_client_t5.o \
		test/rabbitmq/test_simple.o \
		test/rabbitmq/test_work.o \
		test/rabbitmq/test_publish.o \
		test/rabbitmq/test_routing.o


############# SO source file ############# 		
CPPS = bingo/string.cpp \
		bingo/error_what.cpp \
		bingo/configuration/node.cpp \
		bingo/configuration/xml/xml_parser.cpp \
		bingo/configuration/json/json_parser.cpp \
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
		bingo/RabbitMQ/handlers/simple_sendor.cpp \
		bingo/RabbitMQ/handlers/simple_receiver.cpp \
		bingo/RabbitMQ/handlers/work_sendor.cpp \
		bingo/RabbitMQ/handlers/work_receiver.cpp \
		bingo/RabbitMQ/handlers/publish_sendor.cpp \
		bingo/RabbitMQ/handlers/publish_receiver.cpp \
		bingo/RabbitMQ/handlers/routing_sendor.cpp \
		bingo/RabbitMQ/handlers/routing_receiver.cpp \
		bingo/TCP/config/sendor_cfg.cpp \
		bingo/TCP/config/receiver_cfg.cpp
		

############# Debug argument ############# 

# debug_tcp_server
ifeq ($(debug_tcp_server),y)
DEBUG_TCP_SERVER = -DBINGO_TCP_SERVER_DEBUG
else
DEBUG_TCP_SERVER =
endif

# debug_tcp_client
ifeq ($(debug_tcp_client),y)
DEBUG_TCP_CLIENT = -DBINGO_TCP_CLIENT_DEBUG
else
DEBUG_TCP_CLIENT =
endif

# debug_mysql
ifeq ($(debug_mysql),y)
DEBUG_MYSQL = -DBINGO_MYSQL_DEBUG
else
DEBUG_MYSQL =
endif

# debug_thread_task
ifeq ($(debug_thread_task),y)
DEBUG_THREAD_TASK = -DBINGO_THREAD_TASK_DEBUG
else
DEBUG_THREAD_TASK =
endif

# debug_process_task
ifeq ($(debug_process_task),y)
DEBUG_PROCESS_TASK = -DBINGO_PROCESS_SHARED_MEMORY_DEBUG
else
DEBUG_PROCESS_TASK =
endif

# debug_rabbitmq
ifeq ($(debug_rabbitmq),y)
DEBUG_MQ = -DBINGO_MQ_DEBUG
else
DEBUG_MQ =
endif

# define_sart_message_type
ifeq ($(define_sart_message_type),y)
DEFINE_SART_MESSAGE_TYPE = -DBINGO_SART_MESSAGE_TYPE
else
DEFINE_SART_MESSAGE_TYPE =
endif


############# Compile option #############
ifeq ($(findstring Test_Debug,$(ConfigName)),Test_Debug)
	CXXFLAGS =	-O0 -g -Wall -fmessage-length=0 -std=c++11 $(INCLUDE) $(DEBUG_TCP_SERVER) \
									$(DEBUG_TCP_CLIENT) \
									$(DEBUG_THREAD_TASK) \
									$(DEBUG_PROCESS_TASK) \
									$(DEBUG_MYSQL) \
									$(DEBUG_MQ) \
									$(DEFINE_SART_MESSAGE_TYPE)
	TARGET = mytest
else ifeq ($(findstring Lib_Debug,$(ConfigName)),Lib_Debug)
	CXXFLAGS =	-O0 -g -fPIC  -shared -std=c++11 $(INCLUDE) $(DEBUG_TCP_SERVER) \
									$(DEBUG_TCP_CLIENT) \
									$(DEBUG_THREAD_TASK) \
									$(DEBUG_PROCESS_TASK) \
									$(DEBUG_MYSQL) \
									$(DEBUG_MQ) \
									$(DEFINE_SART_MESSAGE_TYPE)
	TARGET = libbingo_v3_d.so
else ifeq ($(findstring Lib_Release,$(ConfigName)),Lib_Release)
	CXXFLAGS =	-O2 -fPIC  -shared -std=c++11 $(INCLUDE) $(DEFINE_SART_MESSAGE_TYPE)
	TARGET = libbingo_v3.so
endif

############# Run to compile  #############
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


