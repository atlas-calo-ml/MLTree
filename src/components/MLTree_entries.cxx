
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MLTreeMaker.h"

DECLARE_ALGORITHM_FACTORY( MLTreeMaker )

DECLARE_FACTORY_ENTRIES( MLTree ) 
{
  DECLARE_ALGORITHM( MLTreeMaker );
}
