#define CLASS_DECLARE_ITEM 0
#define CLASS_DEFINE_VALUE 1
#define CLASS_APPEARANCE_TAG 2
#define CLASS_USER 3
#define CLASS_LEVEL 4
#define CLASS_PLAYER 5
#define CLASS_CITY 6
#define CLASS_TOWN 7
#define CLASS_BUILDING 8
#define CLASS_CHARACTER 9
#define CLASS_CONTAINER 10
#define CLASS_FORMATION 11
#define CLASS_MARKER 12
#define CLASS_MISSILE 13
#define CLASS_PROP 14
#define CLASS_ARMY 15
#define CLASS_TERRAIN_ZONE 16
#define CLASS_COMMAND 17
#define CLASS_ACTION_SEQUENCE 18
#define CLASS_CHARACTER_LADDER 19
#define CLASS_USER_EXTENSION 20
#define CLASS_LEVEL_EXTENSION 21
#define CLASS_PLAYER_EXTENSION 22
#define CLASS_CITY_EXTENSION 23
#define CLASS_TOWN_EXTENSION 24
#define CLASS_BUILDING_EXTENSION 25
#define CLASS_CHARACTER_EXTENSION 26
#define CLASS_CONTAINER_EXTENSION 27
#define CLASS_FORMATION_EXTENSION 28
#define CLASS_MARKER_EXTENSION 29
#define CLASS_MISSILE_EXTENSION 30
#define CLASS_PROP_EXTENSION 31
#define CLASS_ARMY_EXTENSION 32
#define CLASS_DECLARE_ALIAS 33
#define CLASS_ASSOCIATE_CATEGORY 34
#define CLASS_DECLARE_ASSOCIATE_CATEGORY 35
#define CLASS_ORDER 36
#define CLASS_TASK 37
#define CLASS_REACTION 38
#define CLASS_DIPLOMATIC_LADDER 39
#define CLASS_DEFAULT_DIPLOMATIC_STATUS 40
#define CLASS_INDEV_EQUATION 41
#define CLASS_INDEV_OBJECT_FINDER_DEFINITION 42
#define CLASS_INDEV_ACTION_SEQUENCE 43
#define CLASS_EQUATION 44
#define CLASS_OBJECT_FINDER_DEFINITION 45
#define CLASS_TYPE_TAG 46
#define CLASS_OBJECT_CREATION 47
#define CLASS_GAME_EVENT 48
#define CLASS_PACKAGE_RECEIPT_TRIGGER 49
#define CLASS_VALUE_TAG 50
#define CLASS_DEFAULT_VALUE_TAG_INTERPRETATION 51
#define CLASS_ORDER_ASSIGNMENT 52
#define CLASS_ORDER_CATEGORY 53
#define CLASS_TASK_CATEGORY 54
#define CLASS_NUM 55
extern char *CLASS_str[55];
#define CCOMMAND_BUTTON_ENABLED 0
#define CCOMMAND_BUTTON_DEPRESSED 1
#define CCOMMAND_BUTTON_HIGHLIGHTED 2
#define CCOMMAND_BLUEPRINT_TOOLTIP 3
#define CCOMMAND_START_SEQUENCE 4
#define CCOMMAND_NUM 5
extern char *CCOMMAND_str[5];
#define CBLUEPRINT_ITEM 0
#define CBLUEPRINT_STARTS_WITH_ITEM 1
#define CBLUEPRINT_BLUEPRINT_TOOLTIP 2
#define CBLUEPRINT_HAS_DYNAMIC_SHADOW 3
#define CBLUEPRINT_HAS_STATIC_SHADOW 4
#define CBLUEPRINT_MAP_SOUND_TAG 5
#define CBLUEPRINT_OFFERS_COMMAND 6
#define CBLUEPRINT_PHYSICAL_SUBTYPE 7
#define CBLUEPRINT_SCALE_APPEARANCE 8
#define CBLUEPRINT_MOVEMENT_BAND 9
#define CBLUEPRINT_INHERITS_FROM 10
#define CBLUEPRINT_MAP_TYPE_TAG 11
#define CBLUEPRINT_INTRINSIC_REACTION 12
#define CBLUEPRINT_INTERPRET_VALUE_TAG_AS 13
#define CBLUEPRINT_REPRESENT_AS 14
#define CBLUEPRINT_NUM 15
extern char *CBLUEPRINT_str[15];
#define VALUE_CONSTANT 0
#define VALUE_DEFINED_VALUE 1
#define VALUE_ITEM_VALUE 2
#define VALUE_NUM_OBJECTS 3
#define VALUE_EQUATION_RESULT 4
#define VALUE_OBJECT_TYPE 5
#define VALUE_OBJECT_CLASS 6
#define VALUE_OBJECT_ID 7
#define VALUE_IS_SUBSET_OF 8
#define VALUE_WATER_BENEATH 9
#define VALUE_BLUEPRINT_ITEM_VALUE 10
#define VALUE_DISTANCE_BETWEEN 11
#define VALUE_DIPLOMATIC_STATUS_AT_LEAST 12
#define VALUE_CURRENTLY_DOING_ORDER 13
#define VALUE_TOTAL_ITEM_VALUE 14
#define VALUE_HAS_APPEARANCE 15
#define VALUE_CAN_REACH 16
#define VALUE_SAME_PLAYER 17
#define VALUE_ARE_ASSOCIATED 18
#define VALUE_IS_IDLE 19
#define VALUE_IS_DISABLED 20
#define VALUE_NUM 21
extern char *VALUE_str[21];
#define ENODE_ADDITION 0
#define ENODE_SUBTRACTION 1
#define ENODE_MULTIPLICATION 2
#define ENODE_DIVISION 3
#define ENODE_ABSOLUTE_VALUE 4
#define ENODE_MAX 5
#define ENODE_MIN 6
#define ENODE_AND 7
#define ENODE_OR 8
#define ENODE_NOT 9
#define ENODE_IS_ZERO 10
#define ENODE_IS_POSITIVE 11
#define ENODE_IS_NEGATIVE 12
#define ENODE_EQUALS 13
#define ENODE_LESS_THAN 14
#define ENODE_LESS_THAN_OR_EQUAL_TO 15
#define ENODE_GREATER_THAN 16
#define ENODE_GREATER_THAN_OR_EQUAL_TO 17
#define ENODE_RANDOM_UP_TO 18
#define ENODE_RANDOM_INTEGER 19
#define ENODE_RANDOM_RANGE 20
#define ENODE_IF_THEN_ELSE 21
#define ENODE_IS_BETWEEN 22
#define ENODE_NUM 23
extern char *ENODE_str[23];
#define FINDER_SPECIFIC_ID 0
#define FINDER_SELF 1
#define FINDER_SEQUENCE_EXECUTOR 2
#define FINDER_TARGET 3
#define FINDER_PLAYER 4
#define FINDER_CONTROLLER 5
#define FINDER_CREATOR 6
#define FINDER_CANDIDATE 7
#define FINDER_ALIAS 8
#define FINDER_ASSOCIATES 9
#define FINDER_ASSOCIATORS 10
#define FINDER_PLAYERS 11
#define FINDER_LEVEL 12
#define FINDER_PACKAGE_SENDER 13
#define FINDER_TILE_RADIUS 14
#define FINDER_METRE_RADIUS 15
#define FINDER_NEAREST_TO_SATISFY 16
#define FINDER_DISABLED_ASSOCIATES 17
#define FINDER_GRADE_SELECT 18
#define FINDER_GRADE_SELECT_CANDIDATES 19
#define FINDER_AG_SELECTION 20
#define FINDER_UNION 21
#define FINDER_INTERSECTION 22
#define FINDER_CHAIN 23
#define FINDER_ALTERNATIVE 24
#define FINDER_RESULTS 25
#define FINDER_SUBORDINATES 26
#define FINDER_FILTER 27
#define FINDER_FILTER_FIRST 28
#define FINDER_FILTER_CANDIDATES 29
#define FINDER_NEAREST_CANDIDATE 30
#define FINDER_NUM 31
extern char *FINDER_str[31];
#define POSITION_CENTRE_OF_MAP 0
#define POSITION_LOCATION_OF 1
#define POSITION_OUT_AT_ANGLE 2
#define POSITION_NUM 3
extern char *POSITION_str[3];
#define ACTION_REMOVE 0
#define ACTION_CREATE_OBJECT 1
#define ACTION_EXECUTE_SEQUENCE 2
#define ACTION_REPEAT_SEQUENCE 3
#define ACTION_SET_ITEM 4
#define ACTION_INCREASE_ITEM 5
#define ACTION_DECREASE_ITEM 6
#define ACTION_UPON_CONDITION 7
#define ACTION_CREATE_OBJECT_VIA 8
#define ACTION_SEND_EVENT 9
#define ACTION_CLEAR_ALIAS 10
#define ACTION_ASSIGN_ALIAS 11
#define ACTION_UNASSIGN_ALIAS 12
#define ACTION_DISABLE 13
#define ACTION_ENABLE 14
#define ACTION_CLEAR_ASSOCIATES 15
#define ACTION_REGISTER_ASSOCIATES 16
#define ACTION_DEREGISTER_ASSOCIATES 17
#define ACTION_EXECUTE_ONE_AT_RANDOM 18
#define ACTION_TRACE_VALUE 19
#define ACTION_TRACE_FINDER_RESULTS 20
#define ACTION_TERMINATE_THIS_TASK 21
#define ACTION_TRACE 22
#define ACTION_ASSIGN_ORDER_VIA 23
#define ACTION_TERMINATE_TASK 24
#define ACTION_TERMINATE_ORDER 25
#define ACTION_TERMINATE_THIS_ORDER 26
#define ACTION_CANCEL_ORDER 27
#define ACTION_TRANSFER_CONTROL 28
#define ACTION_SWITCH_APPEARANCE 29
#define ACTION_CONVERT_TO 30
#define ACTION_CONVERT_ACCORDING_TO_TAG 31
#define ACTION_EXECUTE_SEQUENCE_AFTER_DELAY 32
#define ACTION_EXECUTE_SEQUENCE_OVER_PERIOD 33
#define ACTION_REPEAT_SEQUENCE_OVER_PERIOD 34
#define ACTION_ADD_REACTION 35
#define ACTION_REMOVE_REACTION 36
#define ACTION_CHANGE_REACTION_PROFILE 37
#define ACTION_SET_SCALE 38
#define ACTION_TERMINATE 39
#define ACTION_STOP_SOUND 40
#define ACTION_PLAY_SOUND 41
#define ACTION_PLAY_SOUND_AT_POSITION 42
#define ACTION_PLAY_SPECIAL_EFFECT 43
#define ACTION_PLAY_ANIMATION_IF_IDLE 44
#define ACTION_ATTACH_SPECIAL_EFFECT 45
#define ACTION_ATTACH_LOOPING_SPECIAL_EFFECT 46
#define ACTION_DETACH_LOOPING_SPECIAL_EFFECT 47
#define ACTION_REVEAL_FOG_OF_WAR 48
#define ACTION_COLLAPSING_CIRCLE_ON_MINIMAP 49
#define ACTION_SHOW_BLINKING_DOT_ON_MINIMAP 50
#define ACTION_NUM 51
extern char *ACTION_str[51];
#define OBJCREATE_TYPE_TO_CREATE 0
#define OBJCREATE_MAPPED_TYPE_TO_CREATE 1
#define OBJCREATE_MATCH_APPEARANCE_OF 2
#define OBJCREATE_CONTROLLER 3
#define OBJCREATE_CREATE_AT 4
#define OBJCREATE_POST_CREATION_SEQUENCE 5
#define OBJCREATE_NUM 6
extern char *OBJCREATE_str[6];
#define SAVEGAME_DELAYED_SEQUENCE_EXECUTION 0
#define SAVEGAME_EXECUTE_SEQUENCE_OVER_PERIOD 1
#define SAVEGAME_REPEAT_SEQUENCE_OVER_PERIOD 2
#define SAVEGAME_GAME_SET 3
#define SAVEGAME_NEXT_UNIQUE_ID 4
#define SAVEGAME_PREDEC 5
#define SAVEGAME_LEVEL 6
#define SAVEGAME_GAME_TYPE 7
#define SAVEGAME_PART_OF_CAMPAIGN 8
#define SAVEGAME_NUM_HUMAN_PLAYERS 9
#define SAVEGAME_UPDATE_ID 10
#define SAVEGAME_NEXT_UPDATE_TIME_STAMP 11
#define SAVEGAME_CURRENT_TIME 12
#define SAVEGAME_PREVIOUS_TIME 13
#define SAVEGAME_ELAPSED_TIME 14
#define SAVEGAME_PAUSED 15
#define SAVEGAME_LOCK_COUNT 16
#define SAVEGAME_CLIENT_STATE 17
#define SAVEGAME_NUM 18
extern char *SAVEGAME_str[18];
#define GAMEOBJ_POSITION 0
#define GAMEOBJ_ORIENTATION 1
#define GAMEOBJ_ITEM 2
#define GAMEOBJ_SUBTYPE 3
#define GAMEOBJ_APPEARANCE 4
#define GAMEOBJ_COLOUR_INDEX 5
#define GAMEOBJ_ORDER_CONFIGURATION 6
#define GAMEOBJ_SCALE 7
#define GAMEOBJ_PLAYER 8
#define GAMEOBJ_CITY 9
#define GAMEOBJ_TOWN 10
#define GAMEOBJ_BUILDING 11
#define GAMEOBJ_CHARACTER 12
#define GAMEOBJ_CONTAINER 13
#define GAMEOBJ_FORMATION 14
#define GAMEOBJ_MARKER 15
#define GAMEOBJ_MISSILE 16
#define GAMEOBJ_PROP 17
#define GAMEOBJ_ARMY 18
#define GAMEOBJ_TERRAIN_ZONE 19
#define GAMEOBJ_MAP 20
#define GAMEOBJ_RECONNAISSANCE 21
#define GAMEOBJ_FOG_OF_WAR 22
#define GAMEOBJ_RECTANGLE 23
#define GAMEOBJ_ALIAS 24
#define GAMEOBJ_ASSOCIATE 25
#define GAMEOBJ_TERMINATED 26
#define GAMEOBJ_PLAYER_TERMINATED 27
#define GAMEOBJ_DISABLE_COUNT 28
#define GAMEOBJ_INDIVIDUAL_REACTION 29
#define GAMEOBJ_DIPLOMATIC_STATUS_BETWEEN 30
#define GAMEOBJ_NAME 31
#define GAMEOBJ_PARAM_BLOCK 32
#define GAMEOBJ_NEXT_UNIQUE_ID 33
#define GAMEOBJ_TILES 34
#define GAMEOBJ_SELECTABLE 35
#define GAMEOBJ_TARGETABLE 36
#define GAMEOBJ_NUM 37
extern char *GAMEOBJ_str[37];
#define OBJTYPE_USER 0
#define OBJTYPE_LEVEL 1
#define OBJTYPE_PLAYER 2
#define OBJTYPE_CITY 3
#define OBJTYPE_TOWN 4
#define OBJTYPE_BUILDING 5
#define OBJTYPE_CHARACTER 6
#define OBJTYPE_CONTAINER 7
#define OBJTYPE_FORMATION 8
#define OBJTYPE_MARKER 9
#define OBJTYPE_MISSILE 10
#define OBJTYPE_PROP 11
#define OBJTYPE_ARMY 12
#define OBJTYPE_TERRAIN_ZONE 13
#define OBJTYPE_NUM 14
extern char *OBJTYPE_str[14];
#define SETTING_GAME_DIR 0
#define SETTING_FAR_Z_VALUE 1
#define SETTING_HARDWARE_VERTEX_PROCESSING 2
#define SETTING_OCCLUSION_RATE 3
#define SETTING_VSYNC 4
#define SETTING_TEXTURE_COMPRESSION 5
#define SETTING_MAP_MAX_PART_SIZE 6
#define SETTING_FOG 7
#define SETTING_FONT 8
#define SETTING_HARDWARE_CURSOR 9
#define SETTING_NUM 10
extern char *SETTING_str[10];
#define STASK_TRIGGER 0
#define STASK_TARGET 1
#define STASK_PROXIMITY 2
#define STASK_PROXIMITY_SATISFIED 3
#define STASK_LAST_DESTINATION_VALID 4
#define STASK_FIRST_EXECUTION 5
#define STASK_TRIGGERS_STARTED 6
#define STASK_PROCESS_STATE 7
#define STASK_TASK_ID 8
#define STASK_START_SEQUENCE_EXECUTED 9
#define STASK_SPAWN_BLUEPRINT 10
#define STASK_COST_DEDUCTED 11
#define STASK_DESTINATION 12
#define STASK_FACE_TOWARDS 13
#define STASK_NUM 14
extern char *STASK_str[14];
#define SORDER_TASK 0
#define SORDER_PROCESS_STATE 1
#define SORDER_CYCLED 2
#define SORDER_ORDER_ID 3
#define SORDER_UNIQUE_TASK_ID 4
#define SORDER_CURRENT_TASK 5
#define SORDER_NUM 6
extern char *SORDER_str[6];
#define PDEVENT_ON_STAMPDOWN 0
#define PDEVENT_ON_LEVEL_START 1
#define PDEVENT_NUM 2
extern char *PDEVENT_str[2];
extern char* PDEVENT_tab0[2];
#define ORDTSKTYPE_OBJECT_REFERENCE 0
#define ORDTSKTYPE_SPAWN 1
#define ORDTSKTYPE_UPGRADE 2
#define ORDTSKTYPE_MOVE 3
#define ORDTSKTYPE_FACE_TOWARDS 4
#define ORDTSKTYPE_NUM 5
extern char *ORDTSKTYPE_str[5];
#define CORDER_USE_TASK 0
#define CORDER_FLAG 1
#define CORDER_CLASS_TYPE 2
#define CORDER_IN_ORDER_CATEGORY 3
#define CORDER_INITIALISATION_SEQUENCE 4
#define CORDER_START_SEQUENCE 5
#define CORDER_SUSPENSION_SEQUENCE 6
#define CORDER_RESUMPTION_SEQUENCE 7
#define CORDER_CANCELLATION_SEQUENCE 8
#define CORDER_TERMINATION_SEQUENCE 9
#define CORDER_NUM 10
extern char *CORDER_str[10];
#define CTASK_CLASS_TYPE 0
#define CTASK_IN_TASK_CATEGORY 1
#define CTASK_TASK_TARGET 2
#define CTASK_PROXIMITY_REQUIREMENT 3
#define CTASK_FLAG 4
#define CTASK_INITIALISATION_SEQUENCE 5
#define CTASK_START_SEQUENCE 6
#define CTASK_SUSPENSION_SEQUENCE 7
#define CTASK_RESUMPTION_SEQUENCE 8
#define CTASK_CANCELLATION_SEQUENCE 9
#define CTASK_TERMINATION_SEQUENCE 10
#define CTASK_PROXIMITY_SATISFIED_SEQUENCE 11
#define CTASK_TRIGGER 12
#define CTASK_NUM 13
extern char *CTASK_str[13];
#define ORDERASSIGNMODE_FORGET_EVERYTHING_ELSE 0
#define ORDERASSIGNMODE_DO_FIRST 1
#define ORDERASSIGNMODE_DO_LAST 2
#define ORDERASSIGNMODE_NUM 3
extern char *ORDERASSIGNMODE_str[3];
#define CORDERASSIGN_ORDER_ASSIGNMENT_MODE 0
#define CORDERASSIGN_ORDER_TO_ASSIGN 1
#define CORDERASSIGN_ORDER_TARGET 2
#define CORDERASSIGN_NUM 3
extern char *CORDERASSIGN_str[3];
#define TASKTRIGGER_TIMER 0
#define TASKTRIGGER_ANIMATION_LOOP 1
#define TASKTRIGGER_ATTACHMENT_POINT 2
#define TASKTRIGGER_COLLISION 3
#define TASKTRIGGER_STRUCK_FLOOR 4
#define TASKTRIGGER_NUM 5
extern char *TASKTRIGGER_str[5];
#define DISTCALCMODE_3D 0
#define DISTCALCMODE_HORIZONTAL 1
#define DISTCALCMODE_VERTICAL 2
#define DISTCALCMODE_NUM 3
extern char *DISTCALCMODE_str[3];
#define OBJFINDCOND_SAME_PLAYER_UNITS 0
#define OBJFINDCOND_ALLIED_UNITS 1
#define OBJFINDCOND_ENEMY_UNITS 2
#define OBJFINDCOND_ORIGINAL_SAME_PLAYER_UNITS 3
#define OBJFINDCOND_ORIGINAL_ALLIED_UNITS 4
#define OBJFINDCOND_ORIGINAL_ENEMY_UNITS 5
#define OBJFINDCOND_BUILDINGS_ONLY 6
#define OBJFINDCOND_CHARACTERS_ONLY 7
#define OBJFINDCOND_CHARACTERS_AND_BUILDINGS_ONLY 8
#define OBJFINDCOND_NUM 9
extern char *OBJFINDCOND_str[9];
