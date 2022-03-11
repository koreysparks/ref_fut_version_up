
CXX=g++
OUT_NAME=rem_fut_version_up

OUT_PATH=./
SRC_PATH=./
OBJ_PATH=./obj/

BOOST=$(BOOST_1_53_0)

INCLUDE_PATH=./ -I$(BOOST)
LIB_PATH=./bin/linux/


LIB_FILE=-ldl -lrt -lmysqlclient\
		-L$(BASE_JYS)  -lbase_jys \
		$(BOOST)/stage/lib_linux64/libboost_thread-mt-d.a \
		$(BOOST)/stage/lib_linux64/libboost_system-mt-d.a \
	


SRC_FILE=$(wildcard $(SRC_PATH) *.cpp)
ALL_FILE=$(notdir $(SRC_FILE))
OBJ_FILE=$(patsubst %.cpp,%.o,$(ALL_FILE))

DEBUG_CXXFLAGS=-g -DEBUG   


OUT_DEBUG_FILE=$(OUT_PATH)$(OUT_NAME)
OBJ_DEBUG_FILE=$(addprefix  $(OBJ_PATH),$(notdir $(OBJ_FILE)))


.PHONY: clean

debug : pre_debug $(OUT_DEBUG_FILE)

pre_debug :
	-$(shell mkdir $(OBJ_PATH) -p)
	-$(shell mkdir $(OUT_PATH) -p)
	
$(OUT_DEBUG_FILE) : $(OBJ_DEBUG_FILE)
	$(CXX) $(DEBUG_CXXFLAGS) $(addprefix  $(OBJ_PATH),$(notdir $^)) -o $@ $(LIB_FILE) 
	
$(OBJ_PATH)%.o : %.cpp
	$(CXX) -c $(DEBUG_CXXFLAGS)  -I $(INCLUDE_PATH)  $(SRC_PATH)$< -o $@
	
	
clean :
	@echo  "make clean"
	$(shell rm $(OBJ_PATH) -rf)
	$(shell rm $(OUT_PATH)$(OUT_NAME) -f)

