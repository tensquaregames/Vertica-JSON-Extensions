#include <Vertica.h>
using Vertica::TransformFunction;
using Vertica::TransformFunctionFactory;

extern "C" {
#include <json/selector.h>
#include <json/slice.h>
}


class JsonArrayUnnest : public TransformFunction {
public:

	virtual void processPartition(Vertica::ServerInterface &,
	                              Vertica::PartitionReader &argReader,
	                              Vertica::PartitionWriter &resWriter)
	{
		do {
			const Vertica::VString &jsonSrc = argReader.getStringRef(0);
			if (jsonSrc.isNull()) {
				continue;
			}

			json_slice_t jsonIn = json_slice_new(jsonSrc.data(), jsonSrc.length());
			json_slice_t jsonOut;
			json_array_iter_t iter;
			if (json_array_iter_init(&iter, &jsonIn) != JSON_ARRAY_ITER_OK) {
				continue;
			}

			for (;;) {
				json_array_iter_result_t result;
				result = json_array_iter_next(&iter, &jsonOut);
				if (result == JSON_ARRAY_ITER_OK) {
					resWriter.getStringRef(0).copy(jsonOut.src, jsonOut.len);
					resWriter.next();
				} else {
					break;
				}
			}
		} while (argReader.next());
	}
};


class JsonArrayUnnestFactory : public TransformFunctionFactory {
public:

	virtual void getPrototype(Vertica::ServerInterface &,
	                          Vertica::ColumnTypes &argTypes,
	                          Vertica::ColumnTypes &resTypes)
	{
		argTypes.addVarchar();
		resTypes.addVarchar();
	}

	virtual void getReturnType(Vertica::ServerInterface &,
	                           const Vertica::SizedColumnTypes &argTypes,
	                           Vertica::SizedColumnTypes &resTypes)
	{
		const Vertica::VerticaType &jsonSrcType = argTypes.getColumnType(0);
		resTypes.addVarchar(jsonSrcType.getStringLength(), "JsonArrayUnnest");
	}

	virtual TransformFunction *createTransformFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, JsonArrayUnnest);
	}
};


RegisterFactory(JsonArrayUnnestFactory);
