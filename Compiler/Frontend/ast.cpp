#include "ast.hpp"

namespace Frontend {
    JSON* expression_to_json(Expression* e, Memory::BaseAllocator* allocator) {
        switch (e->type) {
            case EXPRESSION_TYPE_BOOLEAN: {
                return JSON::Boolean(allocator, e->boolean->value);
            } break;

            case EXPRESSION_TYPE_INTEGER: {
                return JSON::Integer(allocator, e->integer->value);
            } break;

            case EXPRESSION_TYPE_FLOAT: {
                return JSON::Floating(allocator, e->floating->value);
            } break;

            case EXPRESSION_TYPE_STRING: {
                return JSON::String(allocator, e->str->name);
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                JSON* binary_root = JSON::Object(allocator);

                JSON* desc = JSON::Object(allocator);

                desc->object.push("op", JSON::String(allocator, e->binary->operation.sv));
                desc->object.push("left", expression_to_json(e->binary->left, allocator));
                desc->object.push("right", expression_to_json(e->binary->right, allocator));

                binary_root->push("BinaryOp", desc);
                
                return binary_root;
            } break;

            case EXPRESSION_TYPE_UNARY_OPERATION: {
                JSON* unary_root = JSON::Object(allocator);
                
                JSON* desc = JSON::Object(allocator);
                desc->object.push("op", JSON::String(allocator, e->unary->operation.sv));
                desc->object.push("operand", expression_to_json(e->unary->operand, allocator));

                unary_root->push("Unary", desc);
                
                return unary_root;
            } break;

            case EXPRESSION_TYPE_LOGICAL_OPERATION: {
                JSON* logical_root = JSON::Object(allocator);

                JSON* desc = JSON::Object(allocator);
                desc->object.push("op", JSON::String(allocator, e->logical->operation.sv));
                desc->object.push("left", expression_to_json(e->logical->left, allocator));
                desc->object.push("right", expression_to_json(e->logical->right, allocator));

                logical_root->push("Logical", desc);
                
                return logical_root;
            } break;

            case EXPRESSION_TYPE_GROUPING: {
                JSON* grouping_root = JSON::Object(allocator);
                grouping_root->object.push("Grouping", expression_to_json(e->grouping->value, allocator));

                return grouping_root;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return nullptr;
    }

    JSON* decleration_to_json(Decleration* decl, Memory::BaseAllocator* allocator) {
        switch (decl->type) {
            case DECLERATION_TYPE_VARIABLE: {
                JSON* variable_root = JSON::Object(allocator);

                JSON* desc = JSON::Object(allocator);
                desc->push("variable_name", decl->variable->variable_name);
                desc->push("type_name", decl->variable->type_name);
                desc->push("right", expression_to_json(decl->variable->right, allocator));

                variable_root->push("VariableDeceleration", desc);

                return variable_root;
            } break;

            case DECLERATION_TYPE_FUNCTION: {
                JSON* function_root = JSON::Object(allocator);

                JSON* desc = JSON::Object(allocator);
                JSON* body_json = JSON::Array(allocator);
                desc->push("function_name", decl->function->function_name);
                desc->push("return_type", decl->function->return_type_name);

                for (ASTNode* node : decl->function->body) {
                    switch (node->type) {
                        case AST_NODE_EXPRESSION: {
                            /* code */
                        } break;
                        
                        default: {
                            RUNTIME_ASSERT(false);
                        } break;
                    }
                }

                // body_json->array_push();
                /*

                decl->array_push("body", body_json);

                desc->push("right", expression_to_json(decl->variable->right, allocator));

                variable_root->push("VariableDeceleration", desc);
                */

                return function_root;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return nullptr;
    }

    JSON* ast_to_json(ASTNode* node, Memory::BaseAllocator* allocator) {
        switch (node->type) {
            case AST_NODE_PROGRAM: {
                Program* p = node->program;
                JSON* json_root = JSON::Object(allocator);
                JSON* json_declerations = JSON::Array(allocator);

                json_root->push("Declerations", json_declerations);
                for (Decleration* decl : p->declerations) {
                    json_declerations->array_push(decleration_to_json(decl, allocator));
                }

                return json_root;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return nullptr;
    }

    void ASTNode::pretty_print(Memory::BaseAllocator* allocator) {
        LOG_INFO("%s\n", JSON::to_string(ast_to_json(this, allocator)));
    }
}
