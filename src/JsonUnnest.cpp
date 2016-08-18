#include <Vertica.h>
using Vertica::TransformFunction;
using Vertica::TransformFunctionFactory;

extern "C" {
#include <json/selector.h>
#include <json/slice.h>
}


#define CLASS_NAME(CLASS) #CLASS


template<class D>
class AbstractJsonArrayUnnest : public TransformFunction {
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
					D::copyResult(jsonOut, resWriter.getStringRef(0));
					resWriter.next();
				} else {
					break;
				}
			}
		} while (argReader.next());
	}
};

class JsonArrayUnnest : public AbstractJsonArrayUnnest<JsonArrayUnnest> {
public:

	static void copyResult(const json_slice_t &json, Vertica::VString &result)
	{
		result.copy(json.src, json.len);
	}

	static const char *resultColumnName()
	{
		return "JsonArrayUnnest";
	}
};

class JsonArrayUnnestStrings : public AbstractJsonArrayUnnest<JsonArrayUnnestStrings> {
public:

	static void copyResult(const json_slice_t &json, Vertica::VString &result)
	{
		if (json.len >= 2 && json.src[0] == '"' && json.src[json.len - 1] == '"') {
			result.copy(json.src + 1, json.len - 2);
		} else {
			result.setNull();
		}
	}

	static const char *resultColumnName()
	{
		return "JsonArrayUnnestStrings";
	}
};

class JsonArrayUnnestUnquoted : public AbstractJsonArrayUnnest<JsonArrayUnnestUnquoted> {
public:

	static void copyResult(const json_slice_t &json, Vertica::VString &result)
	{
		if (json.len >= 2 && json.src[0] == '"' && json.src[json.len - 1] == '"') {
			result.copy(json.src + 1, json.len - 2);
		} else {
			result.copy(json.src, json.len);
		}
	}
	
	static const char *resultColumnName()
	{
		return "JsonArrayUnnestUnquoted";
	}
};


template<class T>
class AbstractJsonArrayUnnestFactory : public TransformFunctionFactory {
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
		resTypes.addVarchar(jsonSrcType.getStringLength(), T::resultColumnName());
	}

	virtual TransformFunction *createTransformFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, T);
	}
};

class JsonArrayUnnestFactory :
	public AbstractJsonArrayUnnestFactory<JsonArrayUnnest> {
};

class JsonArrayUnnestStringsFactory :
	public AbstractJsonArrayUnnestFactory<JsonArrayUnnestStrings> {
};

class JsonArrayUnnestUnquotedFactory :
	public AbstractJsonArrayUnnestFactory<JsonArrayUnnestUnquoted> {
};


RegisterFactory(JsonArrayUnnestFactory);
RegisterFactory(JsonArrayUnnestStringsFactory);
RegisterFactory(JsonArrayUnnestUnquotedFactory);
