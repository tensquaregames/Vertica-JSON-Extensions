SELECT JsonQuery('{ "foo": "bar baz" }', 'foo');
SELECT JsonQueryString('{ "foo": "bar baz" }', 'foo');
SELECT JsonQueryUnquoted('{ "foo": "bar baz" }', 'foo');

SELECT JsonQueryString('{ "foo": 10 }', 'foo');
SELECT JsonQueryUnquoted('{ "foo": 10 }', 'foo');

SELECT JsonQuery('{ "foo": true }', 'foo')::boolean;
SELECT JsonQueryString('{ "foo": "true" }', 'foo')::boolean;
SELECT JsonQueryUnquoted('{ "foo": true }', 'foo')::boolean;
SELECT JsonQueryUnquoted('{ "foo": "true" }', 'foo')::boolean;
