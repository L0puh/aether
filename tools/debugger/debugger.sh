#!/bin/bash

source tools/base.sh
OPENOCDCFG=tools/debugger/openocd.cfg
GDBCFG=tools/debugger/gdbcfg.gdb

print_help() {
   echo Usage:
   echo "$0 boot - runs gdb debugger for boot ELF"
   echo "$0 app - runs gdb debugger for app EFL"
   echo "$0 all - runs gdb debugger for whole binary"
}

check() {
   say "checking tools..."
   if command -v openocd &> /dev/null;
   then 
      success "openOCD is installed!"
   else 
      error "openOCD is not found. install it to continue."
      return 1
   fi
   
   if command -v arm-none-eabi-gdb &> /dev/null;
   then 
      success "arm-none-eabi-gdb is installed!"
   else 
      error "arm-none-eabi-gdb is not found. install it to continue"
      return 1
   fi

   return 0
}

run_openocd(){
   if [ ! -f ${OPENOCDCFG} ]; then
      error "${OPENOCDCFG} is not found"
   else 
      success "${OPENOCDCFG} is found"
   fi

   say "running openocd..."
   openocd -f ${OPENOCDCFG} &
   OPENOCD_PID=$!

   trap 'kill $OPENOCD_PID 2>/dev/null' EXIT

   for i in {1..2}; do
       if grep -q "Listening on port 3333" /tmp/openocd.log 2>/dev/null; then
           break
       fi
       sleep 0.3
       if ! kill -0 $OPENOCD_PID 2>/dev/null; then
           echo
           error "openOCD failed to start. log:\n$(cat /tmp/openocd.log)"
       fi
   done

   success "openOCD running (pid $OPENOCD_PID)"
}

connect_gdb(){
   if [ ! -f ${GDBCFG} ]; then
      error "${GDBCFG} is not found"
   else 
      success "${GDBCFG} is found"
   fi

   arm-none-eabi-gdb -x ${GDBCFG}

}

setup_config() {
   run_openocd
   connect_gdb 
}

if [ ! $1 ];
then 
   print_help
else 
   say "running debugger for $1."
   check

   if [[ $? -ne 0 ]]; then
      exit 
   else 
      say "setup config..."
      setup_config $1
   fi
fi

