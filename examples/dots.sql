SELECT JsonQuery('{ "foo.bar": true, "foo": { "bar": false } }', 'foo.bar');
SELECT JsonQuery('{ "foo.bar": true, "foo": { "bar": false } }', 'foo\.bar');

SELECT JsonQuery('{ "": true }', '');
SELECT JsonQuery('{ "": { "foo": true } }', '.foo');
SELECT JsonQuerY('{ "": { "": { ".": true } } }', '..\.');
