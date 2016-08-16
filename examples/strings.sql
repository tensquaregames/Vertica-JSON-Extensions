SELECT JsonQuery('{ "foo": "bar baz" }', 'foo');
SELECT JsonQueryString('{ "foo": "bar baz" }', 'foo');

SELECT JsonQuery('{ "foo": true }', 'foo')::boolean;
SELECT JsonQueryString('{ "foo": "true" }', 'foo')::boolean;
