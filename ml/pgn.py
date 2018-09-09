# coding=utf-8

__author__ = 'Cezary Wagner'
__copyright__ = 'Copyright 2015, Cezary Wagner.'
__license__ = 'Commercial'
__version__ = '1.1'

import re


class GameVisitor(object):
    def begin_game(self):
        pass

    def end_game(self):
        pass

    def invalid_game(self):
        pass

    def begin_tags(self):
        pass

    def visit_tag(self, tag_name, tag_value):
        pass

    def end_tags(self):
        pass

    def begin_moves(self):
        pass

    def end_moves(self):
        pass

    def visit_move_number(self, number):
        pass

    def visit_move_san(self, move, check, mark):
        pass

    def visit_game_result(self, result):
        pass


class VerboseGameVisitor(GameVisitor):
    def begin_game(self):
        print 'begin_game'

    def end_game(self):
        print 'end_game'

    def begin_tags(self):
        print 'begin_tags'

    def end_tags(self):
        print 'end_tags'

    def begin_moves(self):
        print 'begin_moves'

    def end_moves(self):
        print 'end_moves'

    def visit_move_number(self, number):
        print 'visit_move_number', number

    def visit_move_san(self, move, check, mark):
        print 'visit_move_san', move, check, mark


class Move(object):
    def __init__(self, san):
        self.san = san


class Game(object):
    def __init__(self):
        self.moves = []
        self.tags = {}

    def add_tag(self, tag, value):
        self.tags[tag] = value

    def add_move(self, move):
        self.moves.append(move)


class PGNFileError(Exception):
    pass


class PGNFile(object):
    def __init__(self, file):
        self.file = file
        self.end_of_file = False    # end of file reached
        self.chars = ''
        self.line = 0

    def _get_chars(self):
        """ Read buffer of line characters. If empty read next line and memorize. """

        # check if empty
        if not self.chars:
            self.chars = self.file.readline()
            # end of file
            if self.chars:
                self.line += 1
            else:
                self.end_of_file = True

        return self.chars

    def _eat_chars(self, chars):
        """ Eat memorized characters after token parsed. """

        # assert len(chars) <= len(self.chars)

        self.chars = self.chars[len(chars):]


    def _match_and_eat_chars(self, reg_expr):
        match = reg_expr.match(self._get_chars())
        if match:
            self._eat_chars(match.group(0))
            return match
        else:
            return None

    TAG_RE = re.compile(r'''
        \[
        ([A-Za-z0-9]+)
        \s+
        "(.*)"
        \]
        \s*\n
    ''', re.VERBOSE)

    EMPTY_LINE_RE = re.compile(r'''
        \s*\n
    ''', re.VERBOSE)

    COMMENT_RE = re.compile(r'''
        %.*\n
    ''', re.VERBOSE)

    SPACES_RE = re.compile(r'''
        \s+
    ''', re.VERBOSE)

    MOVE_NUMBER_RE = re.compile(r'''
        ([1-9][0-9]*)[.]?\s+
    ''', re.VERBOSE)

    MOVE_SAN_RE = re.compile(r'''
        (
          (?:           # figure
          [NBKRQ]       # figure letter
          [a-h1-8]?     # ambiguous
          [x]?          # beats
          [a-h][1-8]    # where
          )
          |(?:          # pawn
          [a-h]?        # ambiguous
          [x]?          # beats
          [a-h][1-8]    # where
          (?:=[NBKRQ])? # promotion
          )
          |(?:O-O)      # king side castling
          |(?:O-O-O)    # queen side castling
        )
        (
          \+            # check
          |\#           # checkmate
        )?
        (
          !             # good move
          |!!           # very good move
          |\?           # poor move
          |\?\?         # very poor move
          |!\?          # speculative move
          |\?!          # questionable move
        )?
        \s+
    ''', re.VERBOSE)

    MOVE_SAN_SKIP_RE = re.compile(r'''
      (\.{3})()()\s+
    ''', re.VERBOSE)

    MOVE_GLYPH_RE = re.compile(r'''
        \$[0-9]+
    ''', re.VERBOSE)

    MOVE_VARIATION_BEGIN_RE = re.compile(r'\(')
    MOVE_VARIATION_END_RE = re.compile(r'\)')

    MOVE_COMMENT_BEGIN_RE = re.compile(r'\{')
    MOVE_COMMENT_END_RE = re.compile(r'\}')

    MOVE_GAME_RESULT_RE = re.compile(r'''
        (1-0       # win
        |0-1      # loose
        |1/2-1/2  # draw
        |\*)\s*
    ''', re.VERBOSE)

    def parse_game(self, visitor):
        """ Return next game in file or None if end of file. """

        def on_begin_game():
            visitor.begin_game()

        def on_end_game():
            visitor.end_game()

        def on_begin_tags():
            visitor.begin_tags()

        def on_end_tags():
            visitor.end_tags()

        def on_begin_moves():
            visitor.begin_moves()

        def on_end_moves():
            visitor.end_moves()

        def on_tag(match):
            visitor.visit_tag(match.group(1),
                              match.group(2))

        def on_move_number(match):
            visitor.visit_move_number(match.group(1))

        def on_move(match):
            visitor.visit_move_san(match.group(1),
                                   match.group(2),
                                   match.group(3))

        def on_game_result(match):
            visitor.visit_game_result(match.group(0))

        def before_tag():
            # empty line
            if self._match_and_eat_chars(self.EMPTY_LINE_RE):
                return before_tag()

            # tag
            match = self._match_and_eat_chars(self.TAG_RE)
            if match:
                on_begin_game()
                on_begin_tags()
                on_tag(match)
                return after_first_tag

            # comment
            if self._match_and_eat_chars(self.COMMENT_RE):
                return before_tag

            if self.end_of_file:
                return None

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        def after_first_tag():
            # tag
            match = self._match_and_eat_chars(self.TAG_RE)
            if match:
                on_tag(match)
                return after_first_tag

            # empty line
            if self._match_and_eat_chars(self.EMPTY_LINE_RE):
                on_end_tags()
                return after_tags

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        def after_tags():
            # move number
            match = self._match_and_eat_chars(self.MOVE_NUMBER_RE)
            if match:
                on_begin_moves()
                on_move_number(match)
                return after_first_move_number

            # move san
            match = self._match_and_eat_chars(self.MOVE_SAN_RE)
            if match:
                on_begin_moves()
                on_move(match)
                return after_white_move_san

            # move white skip san
            match = self._match_and_eat_chars(self.MOVE_SAN_SKIP_RE)
            if match:
                on_begin_moves()
                on_move(match)
                return after_white_move_san

            # move game result
            match = self._match_and_eat_chars(self.MOVE_GAME_RESULT_RE)
            if match:
                on_begin_moves()
                on_game_result(match)
                return after_game_result

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        def after_first_move_number():
            # move san
            match = self._match_and_eat_chars(self.MOVE_SAN_RE)
            if match:
                on_begin_moves()
                on_move(match)
                return after_white_move_san

            # move white skip san
            match = self._match_and_eat_chars(self.MOVE_SAN_SKIP_RE)
            if match:
                on_begin_moves()
                on_move(match)
                return after_white_move_san

            # move game result
            match = self._match_and_eat_chars(self.MOVE_GAME_RESULT_RE)
            if match:
                on_begin_moves()
                on_game_result(match)
                return after_game_result

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        def after_move_number():
            # move san
            match = self._match_and_eat_chars(self.MOVE_SAN_RE)
            if match:
                on_move(match)
                return after_white_move_san

            # move game result
            match = self._match_and_eat_chars(self.MOVE_GAME_RESULT_RE)
            if match:
                on_game_result(match)
                return after_game_result

            # space at start of line
            if self._match_and_eat_chars(self.SPACES_RE):
                return after_move_number()

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        def after_white_move_san():
            # move san
            match = self._match_and_eat_chars(self.MOVE_SAN_RE)
            if match:
                on_move(match)
                return after_black_move_san

            # move game result
            match = self._match_and_eat_chars(self.MOVE_GAME_RESULT_RE)
            if match:
                on_game_result(match)
                return after_game_result

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        def after_black_move_san():
            # move number
            match = self._match_and_eat_chars(self.MOVE_NUMBER_RE)
            if match:
                on_move_number(match)
                return after_move_number

            # move san
            match = self._match_and_eat_chars(self.MOVE_SAN_RE)
            if match:
                on_move(match)
                return after_white_move_san

            # move game result
            match = self._match_and_eat_chars(self.MOVE_GAME_RESULT_RE)
            if match:
                on_game_result(match)
                return after_game_result

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        def after_game_result():
            # empty line
            if self._match_and_eat_chars(self.EMPTY_LINE_RE):
                on_end_moves()
                on_end_game()
                return None

            raise PGNFileError('Error in line %s: %s', self.line, self._get_chars())

        state = before_tag
        while state:
            state = state()

        return True