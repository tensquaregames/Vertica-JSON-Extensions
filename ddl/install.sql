SELECT version();

\set libfile '\''`pwd`'/lib/JsonLib.so\'';
CREATE LIBRARY JsonLib AS :libfile;

CREATE FUNCTION JsonQuery AS LANGUAGE 'C++' NAME 'JsonQueryFactory' LIBRARY JsonLib NOT FENCED;
CREATE FUNCTION JsonQueryString AS LANGUAGE 'C++' NAME 'JsonQueryStringFactory' LIBRARY JsonLib NOT FENCED;
CREATE FUNCTION JsonQueryUnquoted AS LANGUAGE 'C++' NAME 'JsonQueryUnquotedFactory' LIBRARY JsonLib NOT FENCED;
