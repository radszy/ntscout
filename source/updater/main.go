package main

import (
	"archive/zip"
	"bytes"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

const APP_VERSION = "0.1"

var appver *bool = flag.Bool("v", false, "Print the version number.")
var ntsver *float64 = flag.Float64("n", 0.0, "Version of NTScout")
var user *string = flag.String("u", "", "Username used to login.")
var pass *string = flag.String("p", "", "Password used to login.")

var pwd, _ = filepath.Abs(filepath.Dir(os.Args[0]))
var tmp = os.TempDir() + "/NTScout"

var buf bytes.Buffer
var logger = log.New(&buf, "logger: ", log.Lshortfile)

func main() {
	flag.Parse()

	if *appver {
		fmt.Println("Version:", APP_VERSION)
		return
	}

	if *user == "" || *pass == "" || *ntsver == 0.0 {
		fmt.Println("You need to specify username, password and version of NTScout.")
		flag.PrintDefaults()
		return
	}

	logger.Println("Updater version:", APP_VERSION)
	logger.Println("NTScout version:", *ntsver)

	fmt.Print("Updating binary files ")
	if updateBinary() {
		fmt.Println("[X]")
	} else {
		fmt.Println("[ ]")
	}

	fmt.Print("Updating country data ")
	if updateCountry() {
		fmt.Println("[X]")
	} else {
		fmt.Println("[ ]")
	}

	cmd := exec.Command(pwd+"/NTScout", "--post-update")
	err := cmd.Start()
	if err != nil {
		logger.Println(err)
	}

	timestr := strconv.FormatInt(time.Now().Unix(), 10)
	file, err := os.Create("logs/updater-" + timestr)
	if err != nil {
		fmt.Println("Could not create logfile!")
	}
	defer file.Close()
	file.Write(buf.Bytes())
}

func updateCountry() bool {
	if !login(user, pass) {
		return false
	}

	countries := countries()
	if countries == nil {
		return false
	}

	names := namesEn()
	if names == nil {
		return false
	}

	length := len(countries.Country)
	for i := 0; i < length; i++ {
		countries.Country[i].NameEn = names[i]
	}

	file, err := os.OpenFile("country.dat", os.O_WRONLY|os.O_TRUNC, 0777)
	if err != nil {
		logger.Println(err)
		return false
	}
	defer file.Close()

	for _, c := range countries.Country {
		file.WriteString(c.ID + "," + c.Name + "," + c.NameEn + "," +
			c.Divisions + "," + c.Users + "\n")
	}

	return true
}

func updateBinary() bool {
	url := releases()
	if url == "" {
		return false
	}

	if !download(url, os.TempDir()) ||
		!unzip(tmp+".zip", os.TempDir()) ||
		!removeOldFiles() || !moveNewFiles() {
		return false
	}
	clean()

	return true
}

func removeOldFiles() bool {
	entries, err := ioutil.ReadDir(pwd)
	if err != nil {
		logger.Println(err)
		return false
	}

	for _, entry := range entries {
		if entry.Name() == "Updater" {
			continue
		}
		err = os.RemoveAll(pwd + "/" + entry.Name())
		if err != nil {
			logger.Println(err)
			return false
		}
	}

	return true
}

func moveNewFiles() bool {
	entries, err := ioutil.ReadDir(tmp)
	if err != nil {
		logger.Println(err)
		return false
	}

	for _, entry := range entries {
		if entry.Name() == "Updater" {
			err = os.Rename(tmp+"/"+entry.Name(), pwd+"/"+entry.Name()+"-new")
			if err != nil {
				logger.Println(err)
				return false
			}
			continue
		}
		err = os.Rename(tmp+"/"+entry.Name(), pwd+"/"+entry.Name())
		if err != nil {
			logger.Println(err)
			return false
		}
	}

	return true
}

func clean() {
	err := os.RemoveAll(tmp)
	if err != nil {
		logger.Println(err)
	}

	err = os.Remove(tmp + ".zip")
	if err != nil {
		logger.Println(err)
	}
}

func unzip(src, dest string) bool {
	r, err := zip.OpenReader(src)
	if err != nil {
		logger.Println(err)
		return false
	}
	defer r.Close()

	for _, f := range r.File {
		rc, err := f.Open()
		if err != nil {
			logger.Println(err)
			return false
		}
		defer rc.Close()

		fpath := filepath.Join(dest, f.Name)
		if f.FileInfo().IsDir() {
			os.MkdirAll(fpath, f.Mode())
		} else {
			var fdir string
			if lastIndex := strings.LastIndex(fpath, string(os.PathSeparator)); lastIndex > -1 {
				fdir = fpath[:lastIndex]
			}

			err = os.MkdirAll(fdir, f.Mode())
			if err != nil {
				logger.Println(err)
				return false
			}
			f, err := os.OpenFile(
				fpath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
			if err != nil {
				logger.Println(err)
				return false
			}
			defer f.Close()

			_, err = io.Copy(f, rc)
			if err != nil {
				logger.Println(err)
				return false
			}
		}
	}
	return true
}
