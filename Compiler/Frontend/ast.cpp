#include "ast.hpp"

namespace Frontend {
    JSON* ast_to_json(ASTNode* node, JSON* root) {
        switch (root->type) {
            case AST_NODE_EXPRESSION: {
                Expression* e = node->expression;

                switch (e->type) {
                    case EXPRESSION_TYPE_BOOLEAN: {
                        return JSON::Integer(root->allocator, e->boolean->value);
                    } break;

                    case EXPRESSION_TYPE_INTEGER: {
                        return JSON::Integer(root->allocator, e->integer->value);
                    } break;

                    case EXPRESSION_TYPE_BINARY_OPERATION: {
                        JSON* b = JSON::Object(root->allocator);
                        root->object.push("op", JSON::String(b->allocator, e->binary->operation.sv));
                        root->object.push("left", ast_to_json((ASTNode*)e->binary->left, b));
                        root->object.push("right", ast_to_json((ASTNode*)e->binary->right, b));
                        
                        return b;
                    } break;

                    case EXPRESSION_TYPE_GROUPING: {
                        JSON* g = JSON::Object(root->allocator);
                        root->object.push("grouping", ast_to_json((ASTNode*)e->grouping->value, g));

                        return g;
                    } break;
                }
            } break;
        }

        return root;
    }

    void ASTNode::pretty_print(Memory::BaseAllocator* allocator) {
        JSON::Object(allocator);

    }
}
