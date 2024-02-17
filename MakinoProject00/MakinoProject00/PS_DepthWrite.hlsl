// Input
struct INPUT {
    float4 pos : SV_POSITION0;
};

float4 main(INPUT input) : SV_TARGET {
    float depth = input.pos.z;
    return float4(depth, 0.0f, 0.0f, 1.0f);
}
