# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c timer.c video_gr.c bios.c utils.c keyboard.c i8042.h i8254.h sprite.c palette.c button.c mouse.c kbc.c fa.c rtc.c cflap.c manager.c input-box.c mouse-xpm.h uart.c queue.c comm.c title_screen.h

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
