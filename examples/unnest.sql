SELECT JsonArrayUnnest('[1, 2, 3, 4, 5]') OVER ();
SELECT SUM(numbers) FROM (SELECT JsonArrayUnnest('[1, 2, 3, 4, 5]') OVER () AS numbers) AS result;

SELECT JsonArrayUnnest('[1, 2.5, true, false, null, [1, 2, 3], [], {}, { "foo": [3, 4, 5] }]') OVER ();

SELECT JsonArrayUnnest('["foo", "bar", "baz"]') OVER ();
SELECT JsonArrayUnnestStrings('["foo", "bar", "baz"]') OVER ();
SELECT JsonArrayUnnestUnquoted('["foo", "bar", "baz"]') OVER ();

SELECT JsonArrayUnnest('["foo", true, false, "bar"]') OVER ();
SELECT JsonArrayUnnestStrings('["foo", true, false, "bar"]') OVER ();
SELECT JsonArrayUnnestUnquoted('["foo", true, false, "bar"]') OVER ();

SELECT JsonArrayUnnest('["foo", true, 1, 0.1]'::long varchar) OVER ();
SELECT JsonArrayUnnestStrings('["foo", true, 1, 0.1]'::long varchar) OVER ();
SELECT JsonArrayUnnestUnquoted('["foo", true, 1, 0.1]'::long varchar) OVER ();

\set object_json '\'[{ "foo": 4, "bar": true }, { "foo": 8, "bar": false }, { "foo": 16, "bar": true }]\''
SELECT JsonQuery(object, 'foo')
FROM (SELECT JsonArrayUnnest(:object_json) OVER () AS object)
AS selected
WHERE JsonQuery(object, 'bar')::boolean;
