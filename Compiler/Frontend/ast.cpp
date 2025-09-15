#include "ast.hpp"

namespace Frontend {
    JSON* expression_to_json(Expression* e, JSON* root) {
        switch (e->type) {
            case EXPRESSION_TYPE_BOOLEAN: {
                return JSON::Boolean(root->allocator, e->boolean->value);
            } break;

            case EXPRESSION_TYPE_INTEGER: {
                return JSON::Integer(root->allocator, e->integer->value);
            } break;

            case EXPRESSION_TYPE_FLOAT: {
                return JSON::Floating(root->allocator, e->floating->value);
            } break;

            case EXPRESSION_TYPE_STRING: {
                return JSON::String(root->allocator, e->str->name);
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                JSON* b = JSON::Object(root->allocator);
                JSON* left = JSON::Object(root->allocator);
                JSON* right = JSON::Object(root->allocator);
                b->object.push("op", JSON::String(b->allocator, e->binary->operation.sv));
                b->object.push("left", expression_to_json(e->binary->left, left));
                b->object.push("right", expression_to_json(e->binary->right, right));

                root->push("BinaryOp", b);
                
                return root;
            } break;

            case EXPRESSION_TYPE_UNARY_OPERATION: {
                JSON* b = JSON::Object(root->allocator);
                JSON* operand = JSON::Object(root->allocator);
                b->object.push("op", JSON::String(b->allocator, e->unary->operation.sv));
                b->object.push("operand", expression_to_json(e->unary->operand, operand));

                root->push("Unary", b);
                
                return root;
            } break;

            case EXPRESSION_TYPE_LOGICAL_OPERATION: {
                JSON* b = JSON::Object(root->allocator);
                JSON* left = JSON::Object(root->allocator);
                JSON* right = JSON::Object(root->allocator);
                b->object.push("op", JSON::String(b->allocator, e->logical->operation.sv));
                b->object.push("left", expression_to_json(e->logical->left, left));
                b->object.push("right", expression_to_json(e->logical->right, right));

                root->push("Logical", b);
                
                return root;
            } break;

            case EXPRESSION_TYPE_GROUPING: {
                JSON* g = JSON::Object(root->allocator);
                root->object.push("Grouping", expression_to_json(e->grouping->value, g));

                return root;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return root;
    }

    JSON* ast_to_json(ASTNode* node, JSON* root) {
        switch (node->type) {
            case AST_NODE_PROGRAM: {
                // Program* p = node->program;
                // JSON* declerations = JSON::Object(root->allocator);

                // declerations->push("Declerations", )

                // root.push("Program", declerations)

                // for ()


                // return (e, root);
            } break;

            case AST_NODE_EXPRESSION: {
                Expression* e = node->expression;
                return expression_to_json(e, root);
            } break;
                
            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return root;
    }

    void ASTNode::pretty_print(Memory::BaseAllocator* allocator) {
        JSON* root = JSON::Object(allocator);
        LOG_INFO("%s\n", JSON::to_string(ast_to_json(this, root)));
    }
}
