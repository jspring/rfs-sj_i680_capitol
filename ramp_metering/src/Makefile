# Makefile
#
# Copyright (c) 2013   Regents of the University of California
#
include $(CAPATH_MK_DEFS)


#Removed -pg switch in favor of -mfentry. No longer need frame pointer for mcount
#PATH_CFLAGS += -I$(ATSC_INC_DIR) -Wall  -mfentry
PATH_CFLAGS += -I$(ATSC_INC_DIR) -Wall 
PATH_OBJS += $(OBJDIR)/variables2.o
PATH_OBJS += $(OBJDIR)/variables.o
PATH_OBJS += $(OBJDIR)/rm_table.o
PATH_OBJS += $(OBJDIR)/resource.o
PATH_OBJS += $(OBJDIR)/nrutil2.o
PATH_OBJS += $(OBJDIR)/simp1.o
PATH_OBJS += $(OBJDIR)/simp2.o
PATH_OBJS += $(OBJDIR)/simp3.o
PATH_OBJS += $(OBJDIR)/simplx.o
PATH_OBJS += /home/atsc/ab3418/lnx/ab3418_lib.o
#cc  opt_crm.c nrutil2.c simp1.c simp2.c simp3.c simplx.c -o crm_test

PATH_LDFLAGS += -L./lnx -L$(ATSC_LIB_DIR)

EXEC =  $(OBJDIR)/replay_general $(OBJDIR)/wrfiles_rm $(OBJDIR)/clt_vars $(OBJDIR)/opt_crm

all: 	$(OBJDIR) $(PATH_OBJS) $(EXEC)
	/usr/bin/crontab -l >/home/sj_i680_capitol/ramp_metering/system/crontab.txt

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	\rm -f $(OBJDIR)/*.[oa] $(EXEC)
