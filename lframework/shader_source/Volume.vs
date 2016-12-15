#include "Common.vs"
#include "Volume.inc"

//--------------------------------
InOut main(uint vertexID : SV_VertexID)
{
    InOut output;
    output.vertexID = vertexID;
    return output;
}
