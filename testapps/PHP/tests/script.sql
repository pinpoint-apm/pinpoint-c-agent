CREATE TABLE `user` (
        `id` INT NOT NULL AUTO_INCREMENT,
        `name` VARCHAR(100) NOT NULL,
        `age` INT,
        `city` VARCHAR(100),
	PRIMARY KEY (`id`)
);
ALTER TABLE user ADD UNIQUE KEY id_idx (id);
