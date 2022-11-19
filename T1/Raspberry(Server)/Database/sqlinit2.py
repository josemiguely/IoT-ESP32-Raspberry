

#conn = sq.connect("DB.sqlite")
#cur = conn.cursor()

#print("Insertada nueva configuracion")
#conn.close()

import sqlite3 as sql

create_config = """

-- -----------------------------------------------------
-- Table `mydb`.`configuration`
-- -----------------------------------------------------
CREATE TABLE configuration (
  `Id_device` INT ,
  `Status_conf` INT ,
  `Protocol_conf` INT ,
  `Acc_sampling` INT NULL,
  `Acc_sensibility` INT NULL,
  `Gyro_sensibility` INT NULL,
  `BME688_sampling` INT NULL,
  `Discontinuos_time` INT NULL,
  `TCP_PORT` INT NULL,
  `UDP_port` INT NULL,
  `Host_ip_addr` INT NULL,
  `Ssid` VARCHAR(45) NULL,
  `Pass` VARCHAR(45) NULL,
  PRIMARY KEY (`Id_device`))

"""
create_timestamp ="""


-- -----------------------------------------------------
-- Table `mydb`.`timestamps`
-- -----------------------------------------------------
CREATE TABLE timestamps (
  `create_time` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,
  `update_time` TIMESTAMP NULL);

"""
create_logs ="""
-- -----------------------------------------------------
-- Table `mydb`.`Log`
-- -----------------------------------------------------
CREATE TABLE Log (
  `Id_device` INT ,
  `Status_report` INT ,
  `Protocol_report` INT ,
  `Battery Level` INT NULL,
  `Conf_peripheral` INT NULL,
  `Time_client` DATETIME ,
  `Time_server` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,
  `configuration_Id_device` INT NOT NULL,
  PRIMARY KEY (`Id_device`),
  CONSTRAINT `fk_Log_configuration1`
    FOREIGN KEY (`configuration_Id_device`)
    REFERENCES `configuration` (`Id_device`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
"""

create_data1="""
-- -----------------------------------------------------
-- Table `mydb`.`Data_1`
-- -----------------------------------------------------
CREATE TABLE `Data_1` (
  `Id_device` INT NOT NULL,
  `Temperature` INT NULL,
  `Press` INT NULL,
  `Hum` INT NULL,
  `Co` FLOAT NULL,
  `RMS` FLOAT NULL,
  `Amp_x` FLOAT NULL,
  `Freq_x` FLOAT NULL,
  `Amp_y` FLOAT NULL,
  `Freq_y` FLOAT NULL,
  `Amp_z` FLOAT NULL,
  `Freq_z` FLOAT NULL,
  `Time_client` DATETIME ,
  `Log_Id_device` INT NOT NULL,
  PRIMARY KEY (`Id_device`),
  CONSTRAINT `fk_Data_1_Log`
    FOREIGN KEY (`Log_Id_device`)
    REFERENCES `Log` (`Id_device`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
    """

create_data2="""
-- -----------------------------------------------------
-- Table `mydb`.`Data_2`
-- -----------------------------------------------------
CREATE TABLE Data_2 (
  `Id_device` INT NOT NULL,
  `Racc_x` FLOAT NULL,
  `Racc_y` FLOAT NULL,
  `Racc_z` FLOAT NULL,
  `Rgyr_x` FLOAT NULL,
  `Rgyr_y` FLOAT NULL,
  `Rgyr_z` FLOAT NULL,
  `Time_client` DATETIME ,
  `Log_Id_device` INT NOT NULL,
  PRIMARY KEY (`Id_device`),
  CONSTRAINT `fk_Data_2_Log1`
    FOREIGN KEY (`Log_Id_device`)
    REFERENCES `Log` (`Id_device`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)


"""

conn = sql.connect("DBakan.sqlite")
cur = conn.cursor()
cur.execute(create_config)
cur.execute(create_timestamp)
cur.execute(create_logs)
cur.execute(create_data1)
cur.execute(create_data2)
conn.commit()
conn.close()
