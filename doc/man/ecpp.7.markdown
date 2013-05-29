ecpp 7 "2006-07-23" Tntnet "Tntnet users guide"
===============================================



NAME
----

ecpp - template language for tntnet(8)


DESCRIPTION
-----------

`ecpp` is the template-language used by the tntnet-system to generate dynamic
content.

A template consists of normal content (normally html-data) enriched with
special tags, which trigger some special handling.

One ecpp-file is compiled into a C++-class. The C++-class is placed into the
namespace component. A ecpp-file compiled into a C++-class is called component.
The name of the class is the basename of the file.

### `request`, `reply`, `qparam`

Each component has 3 parameters: `request`, `reply` and `qparam`.  `request`
holds information about the client-request like http headers and the url, but
also additional parameters specified in the config-file tntnet.xml(7). The
type of request is `tnt::HttpRequest`.

`reply` receives the answer from the component. The component can set additional
http-headers here, set cookies and - most important - generate output. The most
important methods here are `reply.out()` and `reply.sout()`. Both return a
`std::ostream`, which receives the output of the component. `reply.sout()` has a
filter installed, which translates some characters, whith special meanings in
html to the corresponding html-entities. The characters are `<`, `>`, `&`, `"`
and `'`. This is useful for printing values from variables to the html-code.

`qparam` holds the query-parameters parsed from GET- or POST-parameters or
received from other components. The type of qparam is `tnt::query_params`.
Normally you use a `<%args>`-block to specify the parameters, but there are
special cases, where it is useful to access these directly.

### component adressing

Each component has a unique name. The name is composed from the class-name, the
character '@' and the name of the shared library, it is located. Components can
have internal subcomponents.  The name of the internal subcomponent is appended
to the classname separated by a dot (.).

### special rule for line feeds after a `</%something>`-tag

A line feed immediately after a closing tag for all `<%something>`-blocks are
ignored. Hence blocks followed immediately one after another does not generate
white space in output, which is often undesirable.

### error-handling

Error-handling is done by exception. Tntnet catches all exceptions thrown by
components and handles them properly. Exceptions must be derived from
std::exception. Exceptions derived from `tnt::HttpError`, are handled
separately. They carry a http-return-code, which is sent to the client. Other
exceptions derived from std::exception, result in a http error code 500
(Internal Server Error).

TAGS
----
`<$ expr $>`
  Print expressions expr to the outputstream. The characters `<`, `>`, `&`, `"`
  and `'`, which have special meanings in html, are translated to the
  corresponding html-entities.


`<$$ expr $>`
  Print expressions expr without translating characters with special meaning in
  html to html entities to the output stream.


`<? cond ? expr ?>`
  Conditional output. Print expression expr to the outputstream, if cond
  evaluates to true. Characters with special meaning in html are translated to
  the corresponding html-entities.


`<?? cond ? expr ?>`
  Conditional output. Print expression expr to the outputstream, if cond
  evaluates to true. Characters with special meaning in html are not translated
  to the corresponding html-entities.


`<& component [ arguments ] >`
  Call the specified component. The output of the component is printed into the
  outputstream. If the component-name does not start with a letter, the
  ecpp-compiler treats it as a expression, which returns the name of the
  component. You must surround the expression in brackets, if it contains
  spaces.

  The arguments-part specify the parameters, the component will receive.
  Arguments are name-value-pairs separated by '='. They are put in the
  qparam-parameter of the component and are normally declared in the
  `<%args>`-block. Values can be specified in 3 forms:

  As a plain word without spaces

  As a string enclosed in quotation marks

  As a expression enclosed in brackets

  A single plain word in the argumentlist is treated as a variable of type
  cxxtools::query\_params and a copy is passed to the component. Other
  parameters are added to this copy. If you want to pass all parameters of the
  current component put the variable qparam as a plain word in the argument
  list.


`</&component>`
  Closing-tag for a component-call. When components are called, this
  closing-tag might occur later. The code in <%close>-block is placed here.


`<{...}>`
  C++-inline-processing-block. The code in this block is copied into the
  C++-class unchanged.

  A linefeed after the closing tag is not ignored.


`<#...#>`
  Comment-block. Everything in this block is ignored.


`<%application [ scope="component|page|global" ] >...</%application>`
  Variables defined here, have the lifetime of the application.

  Application-scope is automatically locked.


`<%args>...</%args>`
  Defines GET- or POST-parameters recieved by the component.

  Each argument has a name and optionally a defaul-value. The default-value is
  delimited by '=' from the name. A single argument-definition followed by a
  semicolon (;). In the component a variable with the same name of type
  std::string is defined, which receives the value.

  A argument-name can be prefixed by a type-definition. The ecpp-compiler
  generates code, which tries to convert the value with the
  input-stream-operator. This means, that each type, which can be read from a
  input-stream (std::istream) can be used. If the argument can't be converted,
  a exception is thrown.

  Argumentnames can be postfixed by empty square-brackets. This defines a
  std::vector with the specified type or std::string, if no type is specified.
  This way multiple values with the same name can be received. If a type is
  specified, each value is converted to the target-type.


`<%close>...</%close>`
  Code in these tags is placed into the calling component, when a closing tag
  </&component> is found.

  The <%close> receives the same parameters like the corresponding normal
  component call.


`<%config>...</%config>`
  Often webapplications need some configuration like database-names or
  login-information to the database. These configuratioin-variables can be read
  from the tntnet.xml. Variablenames ended with a semicolon are defined as
  static std::string-variables and filled from tntnet.xml. A variable can be
  prepended by a type. The value from tntnet.xml is then converted with a
  std::istream.

  You can also specify a default value by appending a '=' and the value to the
  variable.

`Example:`

    <%config>
      dburl = "sqlite:db=mydbfile.sqlite";
      int maxvalue = 10;
    </%config>

  tntnet.xml:
    `<dburl>postgresql:dbname=mydb</dburl>`

`<%cpp>...</%cpp>`
  C++-processing-block. The code between these tags are copied into the
  C++-class unchanged.

  A linefeed after the closing tag is ignored.


`<%def name>...</%def>`
  Defines a internal subcomponent with the name name, which can be called like
  other components.


`<%doc>...</%doc>`
  Comment-block. Everything in this block is ignored.

  A linefeed after the closing tag is ignored.


`<%get>...</%get>`
  Works like a <%args> block but receives only GET parameters.


`<%i18n>...</%i18n>`
  Encloses a block of text-data, which is to be translated. See ecppl(1) and
  ecppll(1) for details.


`<%include>filename</%include>`
  The specified file is read and compiled.


`<%param>...</%param>`
  Defines parameter received from calling components. In contrast to
  query-parameters these variables can be of any type. The syntax (and the
  underlying technology) is the same like in scoped variables. See the
  description about scoped variables to see how to define parameters. The main
  difference is, that a parameter variable has no scope, since the parameter is
  always local to the component.


`<%out> expr </%out>`
  Same as <$$ ... $>. Prints the contained C++ expression expr.


`<%post>...</%post>`
  Works like a <%args> block but receives only POST parameters.


`<%pre>...</%pre>`
  Defines C++-code, which is placed outside the C++-class and outside the
  namespace-definition.  This is a good place to define #include-directives.


`<%request [ scope="component|page|global" ] >...</%request>`
  Define request-scope variables. Variables defined here, has the lifetime of
  the request.


`<%session [ scope="component|page|global" ] >...</%session>`
  Variables defined here, has the lifetime of the session.

  Sessions are identified with cookies. If a <%session>-block is defined
  somewhere in a component, a session-cookie is sent to the client.

  Sessions are automatically locked.


`<%securesession [ scope="component|page|global" ] >...</%securesession>`
  Secure session is just like session but a secure cookie is used to identify
  the session. Secure cookies are transfered only over a ssl connection from
  the browser and hence the variables are only kept in a ssl secured
  application.

  If a variable defined here is used in a non ssl page, the variable values are
  lost after the current request.


`<%sout> expr </%sout>`
  Same as <$ ... $>. Prints the contained C++ expression expr. The characters
  <, >, &, " and ', which have special meanings in html, are translated to the
  corresponding html-entities.


`<%thread [ scope="component|page|global" ] >...</%thread>`
  Variables defined here, has the lifetime of the thread. Each thread has his
  own instance of these variables.

  Thread-scope-variables do not need to be locked at all, because they are only
  valid in the current thread.


SCOPED VARIABLES
----------------
Scoped variables are c++-variables, whose lifetime is handled by tntnet. These
variables has a lifetime and a scope. The lifetime is defined by the tag, used
to declare the variable and the scope is passed as a parameter to the tag.

There are 5 different lifetimes for scoped variables:

`request`
  The variable is valid in the current request. The tag is <%request>.

`application`
  The variable is valid in the application. The tag is <%application>. The
  application is specified by the shared-library of the top-level component.

`session`
  The variable is valid for the current session. The tag is <%session>. If at
  least session-variable is declared in the current request, a session-cookie is
  sent to the client.

`thread`
  The variable is valid in the current thread. The tag is <%thread>.

`param`
  The variable receives parameters. The tag is <%param>.

And 3 scopes:

`component`
  The variable is only valid in the same component. This is the default scope.

`page`
  The variable is shared between the components in a single ecpp-file. You can
  specify multiple internal subcomponents in a %def-block. Variables, defined
  in page-scope are shared between these subcomponents.

`global`
  Variables are shared between all components. If you define the same variable
  with global-scope in different components, they must have the same type. This
  is achieved most easily defining them in a separate file and include them
  with a <%include>-block.

  Variables are automatically locked as needed.  If you use session-variables,
  tntnet ensures, that all requests of the same session are serialized. If you
  use application-variables, tntnet serializes all requests to the same
  application-scope. Request- and thread-scope variables do not need to be
  locked at all, because they are not shared between threads.

### Syntax of scoped variables

Scoped variables are declared with exactly the same syntax as normal variables
in c++-code. They can be of any type and are instantiated, when needed.
Objects, which do not have default constructors, need to be specified with
proper constructor-parameters in brackets or separated by '='. The parameters
are only used, if the variable need to be instantiated. This means, that
parameters to e.g. application-scope variables are only used once. When the
same component is called later in the same or another request, the parameters
are not used any more.

### Examples
Specify a application-specific global variable, which is initialized with 0:

    <%application>
    unsigned count = 0;
    </%application>

Specify a variable with a user-defined type, which holds the state of the
session:

    <%session>
    MyClass sessionState;
    </%session>

Specify a persistent databaseconnection, which is initialized, when first needed
and hold for the lifetime of the current thread. This variable may be used in
other components:

    <%thread scope="global">
    tntdb::Connection conn(dburl);
    </%thread>

AUTHOR
------

This manual page was written by Tommi Mäkitalo <tommi@tntnet.org>.

SEE ALSO
--------

tntnet(8), ecppc(1)
