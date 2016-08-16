CREATE TABLE foo (quux varchar(256));

INSERT INTO foo VALUES ('{ "bar": { "norf": 4, "plugh": 3.14 },  "baz": true }');
INSERT INTO foo VALUES ('{ "bar": { "norf": 8, "plugh": 1.73 }, "baz": false }');
INSERT INTO foo VALUES ('{ "bar": { "norf": 15, "plugh": -1.0 }, "baz": false }');
INSERT INTO foo VALUES ('{ "bar": { "norf": 16, "plugh": 111 }, "baz": false }');
INSERT INTO foo VALUES ('{ "bar": { "norf": 23, "plugh": 9.692e3 }, "baz": true }');
INSERT INTO foo VALUES ('{ "bar": { "norf": 42, "plugh": 1337 }, "baz": true }');

SELECT JsonQuery(quux, 'bar.norf')
FROM foo;

SELECT SUM(JsonQuery(quux, 'bar.plugh')::float)
FROM foo WHERE JsonQuery(quux, 'baz')::boolean;

DROP TABLE foo;
