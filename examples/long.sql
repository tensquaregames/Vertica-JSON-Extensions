\set json '\'{ "foo": [4, 8, 15, 16, 23, 42], "bar": { "0": true, "baz": ["quux", "norf"] } }\'::long varchar'

SELECT JsonQuery(:json, 'foo');
SELECT JsonQuery(:json, 'foo.0');
SELECT JsonQuery(:json, 'foo.1');
SELECT JsonQuery(:json, 'foo.4');
SELECT JsonQuery(:json, 'bar.0');
SELECT JsonQuery(:json, 'bar.baz.0');
SELECT JsonQuery(:json, 'bar.baz.1');
SELECT JsonQuery(JsonQuery(JsonQuery(:json, 'bar'), 'baz'), '1');
