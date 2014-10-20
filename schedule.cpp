#include <iostream>

#include "kernel.hpp"
#include "map.hpp"
#include "schedule.hpp"

Schedule::Schedule( Map &map ) : map_ref( map )
{
}

Schedule::~Schedule()
{
   /** nothing to do at the moment **/
}

void
Schedule::init()
{
   for( Kernel *kern : map_ref.all_kernels )
   {
      (this)->scheduleKernel( kern );
   }
}

bool
Schedule::scheduleKernel( Kernel *kernel )
{
   /** does nothing **/
   return( false );
}

void
Schedule::invalidateOutputPorts( Kernel *kernel )
{
   for( auto &port : kernel->output )
   {
      port.invalidate();
   }
}

bool
Schedule::kernelHasInputData( Kernel *kernel )
{
   auto &port_list( kernel->input );
   if( port_list.count() == 0 )
   {
      /** only output ports, keep calling till exits **/
      return( true );
   }
   for( auto &port : kernel->input )
   {
      if( port.size() )
      {
         return( true );
      }
   }
   return( false );
}

bool
Schedule::kernelHasNoInputPorts( Kernel *kernel )
{
   /** assume data check is already complete **/
   for( auto &port : kernel->input )
   {
      if( ! port.is_invalid() )
      {
         return( false );
      }
   }
   return( true );
}
