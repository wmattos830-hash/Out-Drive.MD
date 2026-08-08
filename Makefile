# Makefile para OUT DRIVE (SGDK)

ifdef GDK
GDK_ROOT = $(GDK)
else
GDK_ROOT = $(HOME)/sgdk
endif

# Regras padrao do SGDK
include $(GDK_ROOT)/makefile.gen
