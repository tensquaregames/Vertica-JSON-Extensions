SELECT version();

\set libfile '\''`pwd`'/lib/JsonLib.so\'';
CREATE LIBRARY JsonLib AS :libfile;

CREATE FUNCTION JsonQuery AS LANGUAGE 'C++' NAME 'JsonQueryFactory' LIBRARY JsonLib NOT FENCED;
CREATE FUNCTION JsonQueryString AS LANGUAGE 'C++' NAME 'JsonQueryStringFactory' LIBRARY JsonLib NOT FENCED;
CREATE FUNCTION JsonQueryUnquoted AS LANGUAGE 'C++' NAME 'JsonQueryUnquotedFactory' LIBRARY JsonLib NOT FENCED;
CREATE TRANSFORM FUNCTION JsonArrayUnnest AS LANGUAGE 'C++' NAME 'JsonArrayUnnestFactory' LIBRARY JsonLib NOT FENCED;
CREATE TRANSFORM FUNCTION JsonArrayUnnestStrings AS LANGUAGE 'C++' NAME 'JsonArrayUnnestStringsFactory' LIBRARY JsonLib NOT FENCED;
CREATE TRANSFORM FUNCTION JsonArrayUnnestUnquoted AS LANGUAGE 'C++' NAME 'JsonArrayUnnestUnquotedFactory' LIBRARY JsonLib NOT FENCED;
