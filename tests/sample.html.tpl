<!DOCTYPE html>
<html>
    {{! This is a comment }}
<head>
        <title>{{title}}</title>
    </head>
    <body>

        {{>sample_head.html.tpl}}

        <h1>Wellcome to mustache!</h1>

        <p>Héllo {{name}} of genre {{genre}}</p>

        {{?pets}}
            <p>...and your pets are:</p>
            <ul>
            {{#pets}}
                <li> name : {{name}}, kind : {{kind}} url: {{url}}
                {{?nest}} <h1>nested</h1> {{/nest}}
                {{#nest}}
                    <em>{{name}} {{url}}</em>
                {{/nest}}
                </li>
            {{/pets}}
            </ul>
        {{/pets}}

        {{^undef}}
            {{^undef2}}
                <p>{{url}}</p>
            {{/undef2}}
        {{/undef}}

        {{^pets}}
            <p>...and you have no pets.</p>
        {{/pets}}

        {{?childs}}
            <p>...and your childs are:</p>
            <ul>
            {{#childs}}
                <li>name: {{name}}</li>
            {{/childs}}
            </ul>
        {{/childs}}

        {{^childs}}
            <p>..and you have no childs</p>
        {{/childs}}
        {{>sample_foot.html.tpl}}
    </body>
</html>
