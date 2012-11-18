/**
* This file has been modified from its orginal sources.
*
* Copyright (c) 2012 Software in the Public Interest Inc (SPI)
* Copyright (c) 2012 David Pratt
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
***
* Copyright (c) 2008-2012 Appcelerator Inc.
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include "script_binding.h"

namespace tide
{
    ScriptBinding::ScriptBinding()
        : StaticBoundObject("API.Script")
    {
        /**
         * @tiapi(method=True,name=API.Script.addScriptEvaluator,since=0.7) Adds a script evalutor
         * @tiapi Script evaluators are responsible for matching and evaluating custom <script> types, and preprocessing URLs.
         * @tiapi The rules for evaluation/preprocessing are as follows:
         * @tiapi - When a <script type="text/language"> tag is seen, the first evaluator that returns true to canEvaluate("text/language") will evaluate that script.
         * @tiapi - When a URLRequest is received, the first evaluator that returns true to canPreprocess(URL) will preprocess that URL.
         * @tiapi Evaluators should follow this API:
         * @tiapi - canEvaluate(String mimeType), returns true or false.
         * @tiapi - canPreprocess(String mimeType), returns true or false.
         * @tiapi - evaluate(String mimeType, String name, String sourceCode, Object scope), returns result of evaluation
         * @tiapi - preprocess(String url, Object scope), returns preprocessed content.
         * @tiarg[Object, evaluator] The evaluator to add
         */
        this->SetMethod("addScriptEvaluator", &ScriptBinding::_AddScriptEvaluator);
        
        /**
         * @tiapi(method=True,name=API.Script.removeScriptEvaluator,since=0.7) Removes a script evalutor
         * @tiarg[Object, evaluator] The evaluator to remove
         */
        this->SetMethod("removeScriptEvaluator", &ScriptBinding::_RemoveScriptEvaluator);
        
        /**
         * @tiapi(method=True,name=API.Script.canEvaluate,since=0.7)
         * @tiarg[Object, evaluator] The mimeType to check
         * @tiresult[bool] Whether or not an evaluator can evaluate the passed-in mimeType
         */
        this->SetMethod("canEvaluate", &ScriptBinding::_CanEvaluate);
        
        /**
         * @tiapi(method=True,name=API.Script.canPreprocess,since=0.7)
         * @tiarg[String, url] The URL to check
         * @tiresult[bool] Whether or not an evaluator can preprocess the passed-in URL
         */
        this->SetMethod("canPreprocess", &ScriptBinding::_CanPreprocess);
        
        /**
         * @tiapi(method=True,name=API.Script.evaluate,since=0.7) Evaluates a string of code
         * @tiarg[String, mimeType] The code's mime type (i.e. "text/ruby", "text/php")
         * @tiarg[String, name] The name of the code's origin (usually a filename, or URL)
         * @tiarg[String, code] The actual code
         * @tiarg[Object, scope] global variable scope (i.e. "window")
         * @tiresult[Any] result of the evaluation
         */
        this->SetMethod("evaluate", &ScriptBinding::_Evaluate);
        
        /**
         * @tiapi(method=True,name=API.Script.preprocess,since=0.7)
         * @tiapi Runs an app URL through preprocessing, returning the result as a string
         * @tiarg[String, URL] the URL for this resource (app, ti, and file URLs are accepted)
         * @tiarg[Object, scope] global variable scope to expose into the preprocessed file
         * @tiresult[String] result of the preprocessed code
         */
        this->SetMethod("preprocess", &ScriptBinding::_Preprocess);
    }
    

    void ScriptBinding::_AddScriptEvaluator(const ValueList& args, ValueRef result)
    {
        args.VerifyException("addScriptEvaluator", "o");
        
        TiObjectRef evaluator = args.GetObject(0);
        Script::GetInstance()->AddScriptEvaluator(evaluator);
    }
    
    void ScriptBinding::_RemoveScriptEvaluator(const ValueList& args, ValueRef result)
    {
        args.VerifyException("removeScriptEvaluator", "o");
        
        TiObjectRef evaluator = args.GetObject(0);
        Script::GetInstance()->RemoveScriptEvaluator(evaluator);
    }
    
    void ScriptBinding::_CanEvaluate(const ValueList& args, ValueRef result)
    {
        args.VerifyException("canEvaluate", "s");
        
        const char *mimeType = args.GetString(0).c_str();
        result->SetBool(Script::GetInstance()->CanEvaluate(mimeType));
    }
    
    void ScriptBinding::_CanPreprocess(const ValueList& args, ValueRef result)
    {
        args.VerifyException("canPreprocess", "s");
        
        const char *url = args.GetString(0).c_str();
        result->SetBool(Script::GetInstance()->CanPreprocess(url));
    }
    
    void ScriptBinding::_Evaluate(const ValueList& args, ValueRef result)
    {
        args.VerifyException("evaluate", "s s s o");
        
        const char *mimeType = args.GetString(0).c_str();
        const char *name = args.GetString(1).c_str();
        const char *code = args.GetString(2).c_str();
        TiObjectRef scope = args.GetObject(3);
        result->SetValue(Script::GetInstance()->Evaluate(mimeType, name, code, scope));
    }
    
    void ScriptBinding::_Preprocess(const ValueList& args, ValueRef result)
    {
        args.VerifyException("preprocess", "s o");
        
        const char *url = args.GetString(0).c_str();
        TiObjectRef scope = args.GetObject(1);
        
        AutoPtr<PreprocessData> data = Script::GetInstance()->Preprocess(url, scope);
        TiObjectRef o = new StaticBoundObject();
        o->Set("mimeType", Value::NewString(data->mimeType.c_str()));
        o->Set("data", Value::NewObject(data->data));
        result->SetObject(o);
    }
}
