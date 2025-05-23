# Notes App

@auiexample{app}
Note taking app that demonstrates usage of AListModel, AProperty, user data saving and loading.

@image html Screenshot_20250228_133823.png

The functionality includes loading, saving, creating and deleting notes, as well as marking the UI state as dirty when
changes are made.

# Models

`Note` struct describes a note:

@dontinclude examples/app/notes/src/main.cpp
@skip struct Note
@until };

The JSON representation is described as follows:

@line AJSON


# MainWindow class

The `MainWindow` class is responsible for managing the overall view and logic related to notes.

## Fields

- `mNotes`: a member that holds a list of notes.
- `mCurrentNote`: a property holding a pointer to the currently selected note.
- `mDirty`: a boolean property indicating whether there are unsaved changes in the application.

## Methods

- `load()`: loads notes from a JSON file.
  @snippet examples/app/notes/src/main.cpp load
- `save()`: saves the current list of notes to a JSON file. Also resets the dirty state.
  @snippet examples/app/notes/src/main.cpp save
- `newNote()`: creates a new note with a default title and adds it to the list of notes. Sets this note as the currently
  selected note.
  @snippet examples/app/notes/src/main.cpp newNote
- `deleteCurrentNote()`: deletes the current note, if one is selected. Prompts the user for confirmation before
  proceeding with deletion.
  @snippet examples/app/notes/src/main.cpp deleteCurrentNote
- `markDirty()`: marks the application state as dirty when changes made that require saving. Such a simple operation is
  extracted to a dedicated method, so the signals can be easily connected to.
  @skip markDirty()
  @until }

# UI Components and Layout

@dontinclude examples/app/notes/src/main.cpp
- `TitleTextArea`: derivative of `ATextArea` that focuses next text area when `Enter` is pressed.
  @skip TitleTextArea
  @until };
- 'New note' button: creates a new note using `newNote()`.
- 'Save' button: saves application data via `save()` method. It's `enabled` state is controlled by `mDirty` property.
- 'Delete' button: deletes current note via `deleteCurrentNote()` method. It's `enabled` state is controlled by
  `mCurrentNote` property.
- Previews: contains dynamically generated previews of all notes, allowing users to select the one they want to view
  or edit.
  @snippet examples/app/notes/src/main.cpp scrollarea
  `notePreview()` function generates a single preview for a note.
  @snippet examples/app/notes/src/main.cpp notePreview
- Main area of the application is occupied by note editor generated by `noteEditor()` function. `noteEditor()` consists
  of 2 text areas - one for title, one for contents.
  @snippet examples/app/notes/src/main.cpp noteEditor
