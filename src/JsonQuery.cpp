#include <Vertica.h>
using Vertica::ScalarFunction;
using Vertica::ScalarFunctionFactory;

extern "C" {
#include <json/selector.h>
#include <json/slice.h>
}


class JsonQuery : public ScalarFunction {
public:

	virtual void processBlock(Vertica::ServerInterface &,
	                          Vertica::BlockReader &argReader,
	                          Vertica::BlockWriter &resWriter)
	{
		do {
			const Vertica::VString &jsonSrc = argReader.getStringRef(0);
			const Vertica::VString &querySrc = argReader.getStringRef(1);
			Vertica::VString &resSrc = resWriter.getStringRef();

			json_slice_t jsonIn = json_slice_new(jsonSrc.data(), jsonSrc.length());
			json_slice_t jsonOut;
			if (json_slice_query(&jsonIn,
			                     querySrc.data(), querySrc.length(),
			                     &jsonOut)) {
				resSrc.copy(jsonOut.src, jsonOut.len);
			} else {
				resSrc.setNull();
			}
			resWriter.next();
		} while (argReader.next());
	}
};


class JsonQueryString : public ScalarFunction {
public:

	virtual void processBlock(Vertica::ServerInterface &,
	                          Vertica::BlockReader &argReader,
	                          Vertica::BlockWriter &resWriter)
	{
		do {
			const Vertica::VString &jsonSrc = argReader.getStringRef(0);
			const Vertica::VString &querySrc = argReader.getStringRef(1);
			Vertica::VString &resSrc = resWriter.getStringRef();

			json_slice_t jsonIn = json_slice_new(jsonSrc.data(), jsonSrc.length());
			json_slice_t jsonOut;

			bool status = json_slice_query(&jsonIn,
			                               querySrc.data(), querySrc.length(),
			                               &jsonOut);
			if (status &&
			    jsonOut.len >= 2 &&
			    jsonOut.src[0] == '"' && jsonOut.src[jsonOut.len - 1] == '"') {
				resSrc.copy(jsonOut.src + 1, jsonOut.len - 2);
			} else {
				resSrc.setNull();
			}
			resWriter.next();
		} while (argReader.next());
	}
};


class AbstractJsonQueryFactory : public ScalarFunctionFactory {
public:

	explicit AbstractJsonQueryFactory()
	{
		vol = Vertica::IMMUTABLE;
		strict = Vertica::STRICT;
	}

	virtual void getPrototype(Vertica::ServerInterface &,
	                          Vertica::ColumnTypes &argTypes,
	                          Vertica::ColumnTypes &resTypes)
	{
		argTypes.addVarchar();
		argTypes.addVarchar();
		resTypes.addVarchar();
	}

	virtual void getReturnType(Vertica::ServerInterface &,
	                           const Vertica::SizedColumnTypes &argTypes,
	                           Vertica::SizedColumnTypes &resTypes)
	{
		const Vertica::VerticaType &jsonSrcType = argTypes.getColumnType(0);
		resTypes.addVarchar(jsonSrcType.getStringLength());
	}
};


class JsonQueryFactory : public AbstractJsonQueryFactory {
public:

	virtual ScalarFunction *createScalarFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, JsonQuery);
	}
};


class JsonQueryStringFactory : public AbstractJsonQueryFactory {
public:

	virtual ScalarFunction *createScalarFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, JsonQueryString);
	}
};


RegisterFactory(JsonQueryFactory);
RegisterFactory(JsonQueryStringFactory);
