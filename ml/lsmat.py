import array

import numpy as np
from scipy.sparse import coo_matrix


class LSMATBuilder():

    def __init__(self, col_map={}, row_map={}):
        self.rows = array.array('L')
        self.cols = array.array('L')
        self.data = array.array('f')
        self.y = array.array('L')
        self.id = array.array('L')
        self.col_map = col_map
        self.col_names = []
        self.row_map = row_map

    def _get_next_col_id(self):
        return len(self.col_map)

    def _get_next_row_id(self):
        return len(self.row_map)

    def add(self, row, col, val, fit=True):
        # if columns name not in map
        # add it if it is fitting stage
        # else ignore it
        if col not in self.col_map:
            if fit:
                self.col_map[col] = self._get_next_col_id()
                self.col_names.append(col)
            else:
                return

        if row not in self.row_map:
            self.row_map[row] = self._get_next_row_id()

        self.rows.append(self.row_map[row])
        self.cols.append(self.col_map[col])
        self.data.append(val)
        return

    def add_raw(self, row, col, val, fit=True):
        self.rows.append(row)
        self.cols.append(col)
        self.data.append(val)
        return

    def get_coo_matrix(self):
        # shape = (len(set(self.rows)), len(set(self.cols)))
        return coo_matrix((np.array(self.data),
                           (np.array(self.rows, dtype=np.int),
                            np.array(self.cols, dtype=np.int))))
