import qbs

Module {
    property string scalarProp: "leaf"
    property stringList listProp: ["leaf"]

    Rule {
        inputs: ["rule-input"]
        Artifact {
            filePath: "dummy"
            fileTags: ["rule-output"]
            alwaysUpdated: false
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.silent = true;
            cmd.sourceCode = function() {
                console.info("scalar prop: " + product.moduleProperty("leaf", "scalarProp"));
                console.info("list prop: "
                             + JSON.stringify(product.moduleProperties("leaf", "listProp")));
            }
            return [cmd];
        }
    }
}
