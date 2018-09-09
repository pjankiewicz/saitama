


def parseOneTree(root, index, array_type='double', return_type='double'):
    def ifElse(node):
        if 'leaf_index' in node:
            return 'return ' + str(node['leaf_value']) + ';'
        else:
            condition = 'arr[' + str(node['split_feature']) + ']'
            if node['decision_type'] == 'no_greater':
                condition += ' <= ' + str(node['threshold'])
            else:
                condition += ' == ' + str(node['threshold'])
            left = ifElse(node['left_child'])
            right = ifElse(node['right_child'])
            return 'if ( ' + condition + ' ) { ' + left + ' } else { ' + right + ' }'
    return 'static ' + return_type + ' predictTree' + str(index) + '(' + array_type + '[] arr) { ' + ifElse(root) + ' }'

def parseAllTrees(trees, array_type='double', return_type='double'):
    return '\n\n'.join([parseOneTree(tree['tree_structure'], idx, "int", return_type) for idx, tree in enumerate(trees)]) \
        + '\n\n' + return_type + ' predict(' + array_type + '[] arr) { ' \
        + 'return ' + ' + '.join(['predictTree' + str(i) + '(arr)' for i in range(len(trees))]) + ';' \
        + '}'

