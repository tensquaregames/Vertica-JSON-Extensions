#include <Vertica.h>
using Vertica::ScalarFunction;
using Vertica::ScalarFunctionFactory;

extern "C" {
#include <json/selector.h>
#include <json/slice.h>
}
#include "JsonCopyResult.hpp"


template<class Q>
class AbstractJsonQuery : public ScalarFunction {
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
				Q::copyResult(jsonOut, resSrc);
			} else {
				resSrc.setNull();
			}
			resWriter.next();
		} while (argReader.next());
	}
};

class JsonQuery :
	public JsonValueResult,
	public AbstractJsonQuery<JsonQuery> {
};

class JsonQueryString :
	public JsonStringResult,
	public AbstractJsonQuery<JsonQueryString> {
};

class JsonQueryUnquoted :
	public JsonUnquotedResult,
	public AbstractJsonQuery<JsonQueryUnquoted> {
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

	virtual void getPerInstanceResources(Vertica::ServerInterface &,
	                                     Vertica::VResources &resources)
	{
		resources.nFileHandles = 0;
		resources.scratchMemory = 0;
	}
};

class AbstractJsonQueryLongFactory : public ScalarFunctionFactory {
public:

	virtual void getPrototype(Vertica::ServerInterface &,
	                          Vertica::ColumnTypes &argTypes,
	                          Vertica::ColumnTypes &resTypes)
	{
		argTypes.addLongVarchar();
		argTypes.addVarchar();
		resTypes.addLongVarchar();
	}

	virtual void getReturnType(Vertica::ServerInterface &,
	                           const Vertica::SizedColumnTypes &argTypes,
	                           Vertica::SizedColumnTypes &resTypes)
	{
		const Vertica::VerticaType &jsonSrcType = argTypes.getColumnType(0);
		resTypes.addLongVarchar(jsonSrcType.getStringLength());
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

class JsonQueryUnquotedFactory : public AbstractJsonQueryFactory {
public:

	virtual ScalarFunction *createScalarFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, JsonQueryUnquoted);
	}
};

class JsonQueryLongFactory : public AbstractJsonQueryLongFactory {
public:

	virtual ScalarFunction *createScalarFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, JsonQuery);
	}
};

class JsonQueryStringLongFactory : public AbstractJsonQueryLongFactory {
public:

	virtual ScalarFunction *createScalarFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, JsonQueryString);
	}
};

class JsonQueryUnquotedLongFactory : public AbstractJsonQueryLongFactory {
public:

	virtual ScalarFunction *createScalarFunction(Vertica::ServerInterface &iface)
	{
		return vt_createFuncObj(iface.allocator, JsonQueryUnquoted);
	}
};



RegisterFactory(JsonQueryFactory);
RegisterFactory(JsonQueryStringFactory);
RegisterFactory(JsonQueryUnquotedFactory);
RegisterFactory(JsonQueryLongFactory);
RegisterFactory(JsonQueryStringLongFactory);
RegisterFactory(JsonQueryUnquotedLongFactory);
