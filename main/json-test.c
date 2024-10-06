#include <json-c/json.h>
#include <stdio.h>

/*printing the value corresponding to boolean, double, integer and strings*/
void print_json_value(json_object *jobj){
	printf("type: ");

	switch (json_object_get_type(jobj)) {
	case json_type_boolean:
		printf("bool\n");
		printf("value: %s\n", json_object_get_boolean(jobj) ? "true": "false");
		break;

	case json_type_double:
		printf("double\n");
		printf("value: %lf\n", json_object_get_double(jobj));
		break;

	case json_type_int:
		printf("int\n");
		printf("value: %d\n", json_object_get_int(jobj));
		break;

	case json_type_string:
		printf("string\n");
		printf("value: %s\n", json_object_get_string(jobj));
		break;
	}

}

void json_parse_array(json_object *jobj, char *key) {
	void json_parse(json_object * jobj); /*Forward Declaration*/
	enum json_type type;

	json_object *jarray = jobj; /*Simply get the array*/
	if(key) {
		jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
	}

	int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
	printf("length: %d\n",arraylen);
	int i;
	json_object * jvalue;

	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
		type = json_object_get_type(jvalue);
		if (type == json_type_array) {
			json_parse_array(jvalue, NULL);
		}
		else if (type != json_type_object) {
			printf("value[%d]:\n",i);
			print_json_value(jvalue);
		}
		else {
			json_parse(jvalue);
		}
	}
}

/*Parsing the json object*/
void json_parse(json_object * jobj) {
	json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
		printf("key: %s\n", key);

		switch (json_object_get_type(val)) {
			case json_type_boolean:
			case json_type_double:
			case json_type_int:
			case json_type_string:
				print_json_value(val);
				break;

			case json_type_object:
				printf("json object\n");
				jobj = json_object_object_get(jobj, key);
				json_parse(jobj);
				break;

			case json_type_array:
				printf("type: array\n");
				json_parse_array(jobj, key);
				break;
		}
	}
}

void test_parse(){
//	char * string = "{"
//		"\"sitename\" : \"joys of programming\","
//		"\"categories\" : [ \"c\" , [\"c++\" , \"c\" ], \"java\", \"PHP\" ],"
//		"\"author-details\": { \"admin\": false, \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 } "
//	"}";

	char *string = "{\"method\":\"initialize\",\"jsonrpc\":\"2.0\",\"id\":0,\"params\":{\"rootUri\":\"file:///home/jan/temp\",\"capabilities\":{\"textDocument\":{\"documentSymbol\":{\"labelSupport\":false,\"dynamicRegistration\":false,\"tagSupport\":false,\"hierarchicalDocumentSymbolSupport\":true},\"publishDiagnostics\":{\"dataSupport\":false,\"relatedInformation\":false,\"tagSupport\":{\"valueSet\":\"\"},\"codeDescriptionSupport\":false,\"versionSupport\":false},\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":false},\"completion\":{\"insertTextMode\":1,\"completionItem\":{\"insertTextModeSupport\":{\"valueSet\":[1]},\"insertReplaceSupport\":false,\"snippetSupport\":false,\"commitCharactersSupport\":false,\"resolveSupport\":[],\"preselectSupport\":false,\"deprecatedSupport\":false,\"labelDetailsSupport\":true,\"documentationFormat\":[\"plaintext\"]},\"contextSupport\":false,\"dynamicRegistration\":false},\"declaration\":{\"dynamicRegistration\":false,\"linkSupport\":false}}},\"clientInfo\":{\"name\":\"vim-be-complete\"},\"processId\":3288797,\"trace\":\"verbose\",\"locale\":\"en_GB.UTF-8\"}}";
	char *string1 = "{\"params\":{\"rootUri\":\"file:///home/jan/temp\",\"capabilities\":{\"textDocument\":{\"documentSymbol\":{\"labelSupport\":false,\"dynamicRegistration\":false,\"tagSupport\":false,\"hierarchicalDocumentSymbolSupport\":true},\"publishDiagnostics\":{\"dataSupport\":false,\"relatedInformation\":false,\"tagSupport\":{\"valueSet\":\"\"},\"codeDescriptionSupport\":false,\"versionSupport\":false},\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":false},\"completion\":{\"insertTextMode\":1,\"completionItem\":{\"insertTextModeSupport\":{\"valueSet\":[1]},\"insertReplaceSupport\":false,\"snippetSupport\":false,\"commitCharactersSupport\":false,\"resolveSupport\":[],\"preselectSupport\":false,\"deprecatedSupport\":false,\"labelDetailsSupport\":true,\"documentationFormat\":[\"plaintext\"]},\"contextSupport\":false,\"dynamicRegistration\":false},\"declaration\":{\"dynamicRegistration\":false,\"linkSupport\":false}}},\"clientInfo\":{\"name\":\"vim-be-complete\"},\"processId\":3288797,\"trace\":\"verbose\",\"locale\":\"en_GB.UTF-8\"}}";

	json_object *jobj;

	jobj = json_tokener_parse(string);
//	json_parse(jobj);
	printf("get: %s\n", json_object_get_string(json_object_object_get(jobj, "method")));
	json_object_put(jobj);

	jobj = json_tokener_parse(string);
	printf("get: %s\n", json_object_get_string(json_object_object_get(jobj, "method")));
	json_object_put(jobj);
}

void test_dump(){
	json_object * jobj = json_object_new_object();

//	json_object *jarray = json_object_new_array();
//
//	json_object_array_add(jarray, json_object_new_string("c"));
//	json_object_array_add(jarray, json_object_new_string("c++"));
//	json_object_array_add(jarray, json_object_new_string("php"));
//
//	json_object_object_add(jobj,"Site Name", json_object_new_string("Joys of Programming"));
//	json_object_object_add(jobj,"Technical blog", json_object_new_boolean(1));
//	json_object_object_add(jobj,"Average posts per day", json_object_new_double(2.14));
//	json_object_object_add(jobj,"Number of posts", json_object_new_int(10));
//	json_object_object_add(jobj,"Categories", jarray);
	json_object_object_add(jobj, "none", NULL);

	printf ("json object: %s\n", json_object_to_json_string(jobj));

	json_object_put(jobj);
}

int main() {
	test_parse();
//	test_dump();

	return 0;
}
