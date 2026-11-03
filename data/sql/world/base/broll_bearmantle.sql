-- Delete old entries to ensure a clean state
DELETE FROM `creature_template` WHERE `entry` = 99998;
DELETE FROM `creature_template_model` WHERE `CreatureID` = 99998;

-- Insert Broll Bearmantle with Night Elf model 17351
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `rank`, `unit_class`, `type`, `ScriptName`) 
VALUES (99998, 'Broll Bearmantle', 'Form Customizer', 80, 80, 35, 1, 1, 1.14286, 0, 1, 7, 'npc_druid_forms');

INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) 
VALUES (99998, 0, 17351, 1.0, 1.0, 0);
