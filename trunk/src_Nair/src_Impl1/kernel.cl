__kernel void
vecAdd(__global const int* const a,
       __global const int* const b,
       __global int* const c)
{
     const size_t bx = get_group_id(0);
     const size_t tx = get_local_id(0);
     const unsigned index = bx * 256 + tx;
     c[index] = a[index] + b[index]; 
}
