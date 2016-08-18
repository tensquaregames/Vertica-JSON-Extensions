Vertica JSON extensions
=======================


Overview
--------

This library adds basic support for extracting information from JSON values
stored in your Vertica database. Using a very simple selector syntax you are
able to access data stored in nested JSON arrays and objects with ease.

Assume you have a JSON string with the following contents:

    {
        "foo": true,
        "bar: [1, "baz", { "norf": 42 }],
        "quux": {
            "0": false,
            "1": 10.0
        }
    }

Selector is either a key (to access items of objects) or an index (to access
items of arrays). For the above JSON the selector `foo` will point to `true`,
`bar` to `[1, "baz" { "norf: 42 }]`, `quux` to `{ "0": false, "1": 10.0 }`.
In order to access nested values you use the dot operator in your selector. So,
`bar.0` points to `1`, `bar.1` to `"baz"`, `bar.2` to `{ "norf": 42 }`, `quux.0`
to `false`, and `quux.1` to `10.0`. You can have as many nested levels as you
want - `bar.2.norf` will match `42` and so on.

The selector syntax is available in your SQL code as `JsonQuery` function: its
first parameter is string with a JSON you want to process and the second one
is string with selector used to extract information. For example,

    SELECT JsonQuery('{ "foo": true }')::boolean;

will yield SQL `TRUE` value. Check the `examples/` directory to learn more
about using the selector syntax in your queries.

The extension also comes with a `JsonArrayUnnest` transform function that,
similarly to the `unnest` in PostgreSQL, expands an JSON array values into a
set of rows. For example,

    SELECT JsonArrayUnnest('[1, 2, 3]') OVER ();

will yield

     JsonArrayUnnest
    -----------------
     1
     2
     3
    (3 rows)

Refer to the `examples/unnest.sql` for more sophisticated usage samples.


Building
--------

To build the library, navigate to the folder where this README is located and
run the following command:

    make build

If you wish to build the library with debugging capabilities, you might want
to provide `DEBUG` argument, like this:

    make DEBUG=true build


Installation
------------

Assuming that you have `vsql` command on your `PATH` you can use the `install`
task:

    make install

You can also specify custom `vsql` command used for installation using the
`VSQL` argument:

    make VSQL="vsql -U foo -h bar baz" install

Alternatively, you can load the install script located in `ddl/install.sql`
manually.


Testing
-------

To verify that everything works as expected, you can use the following commands:

    make test
    make memtest
    make examples

Note that the `memtest` task requires Valgrind to be available on your `PATH`.


FAQ
---

##### How fast is it?

The library performs absolutely no memory allocations except for the string
used as returned result. It uses a tiny, custom JSON parsing library that
does not actually build any syntax tree. Instead, it "moves" directly in the
JSON string along the selector path and returns offset to the memory fragment.
So, it should be sufficiently fast.

##### I did `make memtest` and Valgrind told me that the JSON library made some allocations. Why?

The `test/json_slice` suite loads three JSON test files and needs to allocate
memory for them. You can comment them out to see that inline tests do not
allocate anything.

##### What if my JSON object has keys with dot characters?

You can escape dots in selectors, so selector `foo\.bar` will match true for
`{ "foo.bar": true }`. Refer to `examples/dots.sql` to see more weird cases.

##### What if my JSON object has empty keys?

Then empty selector will match it. For example, to access `true` in
`{ "foo": { "": { "bar": true } } }` you can use selector `foo..bar`. Refer to
`examples/dots.sql` to see more weird cases.

##### `JsonQuery` returns quoted strings. Why?

`JsonQuery` takes a JSON value (as a string object) and returns a JSON value
(as a string object). So, `JsonQuery('{ "foo": true }', 'foo')` will yield
a string with JSON boolean value `true` and `JsonQuery('{ "foo": "true" }')`
will yield a string with JSON string value `"true"` (with quotemarks).

It makes sense but can be very inconvenient to work with. To solve this, there
is a `JsonQueryString` function that operates almost exactly as `JsonQuery` but
returns unquoted strings (and fails with `NULL` if the selected value is not a
string). For example, `JsonQueryString('{ "foo": "bar" }, 'foo')` will yield an
SQL string with value `bar` (without unnecessary quotes).

Finally, there is also a `JsonQueryUnquoted` that unquotes the returned results
if it is a string value but does not fail if it is not a string. So, for string
values `JsonQueryString` and `JsonQueryUnquoted` behave the same way but for
other JSON values (integers, booleans, arrays) the former one will fail whereas
the latter one will yield the value which may or may not be what you want,
depending on your use.case.

Refer to `examples/strings.sql` to see more usage samples.
