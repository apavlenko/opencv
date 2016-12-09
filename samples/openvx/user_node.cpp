#include "ivx.hpp"

int main()
{
    return 0;
}

using namespace ivx;
const int XYZ_VALUE_MIN = 0, XYZ_VALUE_MAX = 100;
const int XYZ_TEMP_NUMITEMS = 100;

typedef enum _xyz_params_e {
    XYZ_PARAM_INPUT = 0,
    XYZ_PARAM_VALUE,
    XYZ_PARAM_OUTPUT,
    XYZ_PARAM_TEMP
} xyz_params_e;


#ifndef VX_VERSION_1_1

vx_status VX_CALLBACK XYZInputValidator(vx_node node, vx_uint32 index);
vx_status XYZInputValidator(vx_node node, vx_uint32 index)
{
    try
    {
        Param param = Node(node, true).getParam(index);

        if(index == XYZ_PARAM_INPUT)
        {
            Image img( param.reference() );
            if(img.format() != VX_DF_IMAGE_U8)
                throw RuntimeError(VX_ERROR_INVALID_VALUE);
        }
        else if(index == XYZ_PARAM_VALUE)
        {
            Scalar sc( param.reference() );
            if(sc.type() != VX_TYPE_INT32)
                throw RuntimeError(VX_ERROR_INVALID_VALUE);
            vx_int32 val = sc.getValue<vx_int32>();
            if(val < XYZ_VALUE_MIN || val > XYZ_VALUE_MAX)
                throw RuntimeError(VX_ERROR_INVALID_VALUE);
        }
        else if(index == XYZ_PARAM_TEMP)
        {
            Array ar( param.reference() );
            if(ar.itemCount() < XYZ_TEMP_NUMITEMS)
                throw RuntimeError(VX_ERROR_INVALID_VALUE);
        }
        else
            throw RuntimeError(VX_ERROR_INVALID_PARAMETERS);
    }
    catch(const RuntimeError& e)
    { return e.status(); }

    catch(const WrapperError&)
    { return VX_FAILURE; }

    return VX_SUCCESS;
}

vx_status VX_CALLBACK XYZOutputValidator(vx_node node, vx_uint32 index, vx_meta_format meta);
vx_status XYZOutputValidator(vx_node node, vx_uint32 index, vx_meta_format meta)
{
    try
    {
        if (index == XYZ_PARAM_OUTPUT)
        {
            Param param = Node(node, true).getParam(XYZ_PARAM_INPUT);
            Image img( param.reference() );
            MetaFormat mf(meta, true);
#ifdef VX_VERSION_1_1
            mf.setFrom(img);
#else
            mf.set(VX_IMAGE_WIDTH,  img.width());
            mf.set(VX_IMAGE_HEIGHT, img.height());
            mf.set(VX_IMAGE_FORMAT, img.format());
#endif // VX_VERSION_1_1
        }
    }
    catch(const RuntimeError& e)
    { return e.status(); }

    catch(const WrapperError&)
    { return VX_FAILURE; }

    return VX_SUCCESS;
}

#else // VX_VERSION_1_1
// TBD
#endif // VX_VERSION_1_1

/*
vx_status VX_CALLBACK XYZInitialize(vx_node node, const vx_reference *parameters, vx_uint32 num)
{ return VX_SUCCESS; }

vx_status VX_CALLBACK XYZDeinitialize(vx_node node, const vx_reference *parameters, vx_uint32 num)
{ return VX_SUCCESS; }
*/

VX_API_ENTRY vx_status VX_API_CALL vxPublishKernels(vx_context context);
vx_status VX_API_CALL vxPublishKernels(vx_context context)
{
    Kernel k = Context(context, true).addKernel(
        "org.khronos.example.xyz",
        VX_KERNEL_KHR_XYZ,
        XYZKernel,
        4,
        XYZInputValidator,
        XYZOutputValidator
    );

    if (kernel)
    {
        vx_size size = XYZ_DATA_AREA;
        status = vxAddParameterToKernel(kernel, 0, VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED);
        if (status != VX_SUCCESS) goto exit;
        status = vxAddParameterToKernel(kernel, 1, VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED);
        if (status != VX_SUCCESS) goto exit;
        status = vxAddParameterToKernel(kernel, 2, VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED);
        if (status != VX_SUCCESS) goto exit;
        status = vxAddParameterToKernel(kernel, 3, VX_OUTPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED);
        if (status != VX_SUCCESS) goto exit;
        status = vxSetKernelAttribute(kernel, VX_KERNEL_ATTRIBUTE_LOCAL_DATA_SIZE, &size, sizeof(size));
        if (status != VX_SUCCESS) goto exit;
        status = vxFinalizeKernel(kernel);
        if (status != VX_SUCCESS) goto exit;
    }
exit:
    if (status != VX_SUCCESS) {
        vxRemoveKernel(kernel);
    }
    return status;
}

vx_status VX_CALLBACK XYZKernel(vx_node node, const vx_reference *parameters, vx_uint32 num)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (num == 4)
    {
        vx_image input  = (vx_image)parameters[0];
        vx_scalar scalar = (vx_scalar)parameters[1];
        vx_image output = (vx_image)parameters[2];
        vx_array temp  = (vx_array)parameters[3];
        void *buf, *in = NULL, *out = NULL;
        vx_uint32 y, x;
        vx_int32 value = 0;
        vx_imagepatch_addressing_t addr1, addr2;
        vx_rectangle_t rect;
        vx_enum item_type = VX_TYPE_INVALID;
        vx_size num_items = 0, capacity = 0;
        vx_size stride = 0;

        status = VX_SUCCESS;

        status |= vxReadScalarValue(scalar, &value);
        status |= vxGetValidRegionImage(input, &rect);
        status |= vxAccessImagePatch(input, &rect, 0, &addr1, &in, VX_READ_ONLY);
        status |= vxAccessImagePatch(output, &rect, 0, &addr2, &out, VX_WRITE_ONLY);
        status |= vxQueryArray(temp, VX_ARRAY_ATTRIBUTE_ITEMTYPE, &item_type, sizeof(item_type));
        status |= vxQueryArray(temp, VX_ARRAY_ATTRIBUTE_NUMITEMS, &num_items, sizeof(num_items));
        status |= vxQueryArray(temp, VX_ARRAY_ATTRIBUTE_CAPACITY, &capacity, sizeof(capacity));
        status |= vxAccessArrayRange(temp, 0, num_items, &stride, &buf, VX_READ_AND_WRITE);
        for (y = 0; y < addr1.dim_y; y+=addr1.step_y)
        {
            for (x = 0; x < addr1.dim_x; x+=addr1.step_x)
            {
                // do some operation...
            }
        }
        // write back and release
        status |= vxCommitArrayRange(temp, 0, num_items, buf);
        status |= vxCommitImagePatch(output, &rect, 0, &addr2, out);
        status |= vxCommitImagePatch(input, NULL, 0, &addr1, in); // don't write back into the input
    }
    return status;
}
